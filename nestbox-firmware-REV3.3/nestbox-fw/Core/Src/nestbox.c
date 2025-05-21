/*
 * nestbox.c
 *
 *  Created on: 15 Sep 2021
 *      Author: raffael
 */

#include "main.h"
#include "load_cell.h"
#include "rfid.h"
#include "comm.h"
#include "nestbox.h"
#include "rtc.h"
#include "system.h"
#include "cli.h"
#include "boot.h"
#include "tim.h"
#include "calibration.h"

typedef enum nbStates{
	// startup
	NB_WAKEUP_GET_TIME = 0,

	// after successfully receiving the sigfox downlink message, or manually with CLI during factory testing:
	NB_COMPLETE_STARTUP,

	// normal operation states:
	NB_IDLE_POLLING,
	NB_READ_RFID_TAG,
	NB_MEASURE_WEIGHT,
	NB_WAIT_END_OF_EVENT,
	NB_PROCESS_AND_SEND, // if needed, waits for bird to leave.

//	// periodic states:
//	NB_MEASURE_OFFSET,
//	NB_STATUS_UPDATE,

	// special states:
	NB_RFID_READ_FAILED,

	// power-safe or power-down:
	NB_DAYTIME_SLEEP,		// during the day, measurements are off.
	NB_SHUTDOWN		// if battery voltage is too low, turn off system until reboot.

} nestbox_state_t;

static nestbox_state_t nb_state = NB_WAKEUP_GET_TIME;

static inline void goto_state(nestbox_state_t new_state)
{
	__disable_irq();
	nb_state = new_state;
	__enable_irq();
}

static int nestbox_cli_req = 0;
static int nestbox_usb_state_update_req = 0;

void nestbox_request_cli_process()
{
	nestbox_cli_req++ ;
}

void nestbox_request_usb_state_change()
{
	nestbox_usb_state_update_req = 1;
}


static void nestbox_nap(unsigned int seconds)
{
#ifdef USE_LOW_POWER_MODE
//	system_sleep_IT(1); // used to be here when MCU did not correctly go to sleep. This bug seems fixed no so the line is no longer needed.
	system_sleep_IT(seconds);
#else
	// Periodic sleep
	HAL_Delay(1000 * seconds);
#endif
}

static void nestbox_daytime_sleep()
{
	// go to low power mode sleep and wait for RTC to wake-up system
	system_daytime_sleep(); // function will resume here.

	system_update_usb_connection_state_blocking();

	if(/*system_get_usb_connection_state() == USB_DISCONNECTED &&*/ // if system was woken up by connecting a USB cable, do not initiate a sigfox downlink.
			system_time_to_sleep() == 0) 						// make sure time is right to wake up.
		goto_state(NB_WAKEUP_GET_TIME);
	else
	{
		goto_state(NB_IDLE_POLLING);
		rtc_start_periodic_timer(NB_LOAD_CELL_POLLING_PERIOD); // already done in state NB_WAKEUP_GET_TIME
	}
}

static void nestbox_shutdown()
{
	system_shutdown();
}


// periodic timers:
static unsigned int nb_offset_meas_timestamp = 0;
static unsigned int nb_offset_tx_timestamp = 0;
static unsigned int nb_status_meas_timestamp = 0;
//static unsigned int nb_status_tx_timestamp = 0;
static unsigned int nb_get_time_tx_timestamp = 0;

// timeout counters
static unsigned int rfid_retries_counter		= 0;
static unsigned int system_info_invalid_counter = 0;

static inline void nestbox_reset_offset_timer()
{
	nb_offset_meas_timestamp = rtc_get_timestamp();
}

static inline void nestbox_reset_contact_base_timer()
{
	nb_get_time_tx_timestamp = rtc_get_timestamp();
//	nb_status_tx_timestamp = rtc_get_timestamp();	// because status is also sent in "contact base message", reset the status timer as well!
}

void nestbox_periodic_tasks()
{
	if(rtc_get_timestamp() - nb_status_meas_timestamp > NB_STATUS_MEAS_PERIOD)
	{
		nb_status_meas_timestamp = rtc_get_timestamp();

		if(system_measure_status(VBAT_SHUTDOWN_MV) == SYSTEM_POWER_DOWN)
			goto_state(NB_SHUTDOWN);
#ifdef NB_DAYTIME_POWER_OFF
		else if(system_time_to_sleep())
			goto_state(NB_DAYTIME_SLEEP);
#endif
		else // status is logged before shutdown or daytime sleep.
			comm_send_status(COMM_UPLINK_ONLY, LOG_TO_MEM_ONLY);
	}

	if(rtc_get_timestamp() - nb_offset_meas_timestamp > NB_OFFSET_MEAS_PERIOD &&
			system_get_usb_connection_state() == USB_DISCONNECTED) // avoid offset measurements while usb is connected
	{
		nestbox_reset_offset_timer();
		load_cell_offset_response_t ofsres = load_cell_measure_offset();
		if(		/*(ofsres == LOAD_CELL_OFFSET_SIGNIFICANT_CHANGE &&
					rtc_get_timestamp() - nb_offset_tx_timestamp > NB_OFFSET_MIN_TX_PERIOD) ||*/
				rtc_get_timestamp() - nb_offset_tx_timestamp > NB_OFFSET_MAX_TX_PERIOD) // to remove "outlier bias" during testing phase, a regular interval is selected at which the offset is transmitted.
		{
			comm_send_offset(LOG_TO_MEM_AND_TX);
			nb_offset_tx_timestamp = rtc_get_timestamp();
		}
		else
		{
			comm_send_offset(LOG_TO_MEM_ONLY);
		}
	}

	if(system_info_invalid() && system_info_invalid_counter < NB_GET_TIME_RETRY_TIMEOUT)
	{
		if(rtc_get_timestamp() - nb_get_time_tx_timestamp > NB_GET_TIME_RETRY_PERIOD &&
				system_get_usb_connection_state() == USB_DISCONNECTED) // avoid long delays while usb is connected
		{
			// if no valid message received from base station: retry fetching it, but max. 3 times
			nb_get_time_tx_timestamp = rtc_get_timestamp();
			comm_wakeup_get_time_from_basestation();
			system_info_invalid_counter++;
		}
	}
}

void nestbox_start_new_event(nestbox_event_t* evt)
{
	nestbox_reset_offset_timer();

	evt->start_timestamp = rtc_get_timestamp();
	evt->flags = 0;
}

// can be called from CLI during factory testing to allow calibration mode.
void nestbox_request_complete_startup()
{
	goto_state(NB_COMPLETE_STARTUP);
}

void nestbox_main()
{
	boot_reset_cause_t rst_c = boot_get_last_reset_cause();
	static nestbox_event_t nb_evt;
	static int led_toggle_on = 1;

	if(system_init() == SYSTEM_POWER_DOWN)
	{
		HAL_Delay(5000); // avoid debugger getting caught
		goto_state(NB_SHUTDOWN);
	}
	else
	{
		HAL_GPIO_WritePin(DEBUG_LED_RTC_CALIB_GPIO_Port, DEBUG_LED_RTC_CALIB_Pin, DEBUG_LED_ON);

		comm_init();
		load_cell_init();

		buzzer_on();
		HAL_Delay(200);
		buzzer_off();
		HAL_Delay(100);
		buzzer_on();
		HAL_Delay(100);
		buzzer_off();
	}

	while(1)
	{
		if(nb_state == NB_WAKEUP_GET_TIME)
		{
			if(system_get_usb_connection_state() != USB_CONNECTED) // do not perform wake-up-routine while USB is connected (to allow quicker testing)
			{
				comm_wakeup_get_time_from_basestation(); // TODO-OK: this updates the RTC and will mess with all timeouts!

				goto_state(NB_COMPLETE_STARTUP);
			}
		}
		if(nb_state == NB_COMPLETE_STARTUP)
		{
			// avoid a new offset measurement takes place directly after POR (already done in load_cell_init())
			nestbox_reset_offset_timer();
			nestbox_reset_contact_base_timer();
			load_cell_reset_calib_timeout();


			load_cell_start_polling_mode();
			rtc_start_periodic_timer(NB_LOAD_CELL_POLLING_PERIOD);

			cli_complete_action();
			goto_state(NB_IDLE_POLLING);
		}
		if(nb_state == NB_IDLE_POLLING)
		{
			// Blink while load cell still in calib mode:
			if(load_cell_get_state() == LOAD_CELL_CALIB && led_toggle_on)
			{
				HAL_GPIO_WritePin(DEBUG_LED_RTC_CALIB_GPIO_Port, DEBUG_LED_RTC_CALIB_Pin, DEBUG_LED_ON);
				led_toggle_on = 0;
			}
			else
			{
				HAL_GPIO_WritePin(DEBUG_LED_RTC_CALIB_GPIO_Port, DEBUG_LED_RTC_CALIB_Pin, DEBUG_LED_OFF);
				led_toggle_on = 1;
			}

			nestbox_periodic_tasks();	// ensure periodic tasks are always executed when idle

			if(load_cell_poll() == LOAD_CELL_ABOVE_THRESHOLD &&
					load_cell_poll() == LOAD_CELL_ABOVE_THRESHOLD)	// sanity check: need to be above threshold twice!
			{
				calib_inform_weight_detected();
				nestbox_start_new_event(&nb_evt);

				if(rfid_retries_counter > NB_RFID_RETRY_TIMEOUT)
					nestbox_nap(NB_LOAD_CELL_POLLING_PERIOD);
				else if(rfid_detect_id() == RFID_TIMEOUT_OR_FAILED)
				{
					if(load_cell_get_state() == LOAD_CELL_CALIB)
					{
						// In calibration state, we don't require a tag on the weights:
						nb_evt.rfid_uid = 0;

						load_cell_start_measurement_mode(&nb_evt);
						load_cell_acquire_next_sample();
						goto_state(NB_MEASURE_WEIGHT);
					}
					else
						goto_state(NB_RFID_READ_FAILED);
				}
				else
				{
					if(load_cell_get_state() == LOAD_CELL_CALIB)
					{
						// if we are still in calibration mode, change state here as a first RFID tag was placed on the perch:
						load_cell_calib_response_t end_calib_result = load_cell_end_calibration_mode();
						switch(end_calib_result)
						{
						case LOAD_CELL_CALIBRATION_LOADED_DEFAULT:
							for(int i=0; i<10; i++)
							{
								// signal an error to the user:
								HAL_Delay(200);
								HAL_GPIO_WritePin(DEBUG_LED_RTC_CALIB_GPIO_Port, DEBUG_LED_RTC_CALIB_Pin, DEBUG_LED_ON);
								HAL_Delay(200);
								HAL_GPIO_WritePin(DEBUG_LED_RTC_CALIB_GPIO_Port, DEBUG_LED_RTC_CALIB_Pin, DEBUG_LED_OFF);
							}
						case LOAD_CELL_CALIBRATION_LOADED_FROM_MEMORY:
						case LOAD_CELL_CALIBRATION_COMPLETED:
							HAL_GPIO_WritePin(DEBUG_LED_RTC_CALIB_GPIO_Port, DEBUG_LED_RTC_CALIB_Pin, DEBUG_LED_OFF);
							buzzer_on();
							HAL_Delay(300);
							buzzer_off();
							HAL_Delay(100);
							buzzer_on();
							HAL_Delay(100);
							buzzer_off();

							// send calibration:
							comm_send_calibration();
							// backup state and logs:
							boot_create_backup();
							break;
						}
					}
					rfid_get_compressed_id(&nb_evt);
					system_increment_events_with_id();

					load_cell_start_measurement_mode(&nb_evt);
					load_cell_acquire_next_sample();
					goto_state(NB_MEASURE_WEIGHT);
				}
			}
			else
			{
				calib_inform_weight_removed();
				rfid_retries_counter = 0;
				nestbox_nap(NB_LOAD_CELL_POLLING_PERIOD);
			}
		}
		if(nb_state == NB_RFID_READ_FAILED)
		{
			if(rtc_get_timestamp() - nb_evt.start_timestamp > NB_RFID_RETRY_PERIOD)
			{
				rfid_retries_counter++;
				if(rfid_retries_counter > NB_RFID_RETRY_TIMEOUT)
				{
					// If timeout is reached, log an unsuccessful ID readout event.
					system_increment_events_unknown_id();

					// still measure the weight and log to flash:
					nb_evt.rfid_uid = 0;

					load_cell_start_measurement_mode(&nb_evt);
					load_cell_acquire_next_sample();
					goto_state(NB_MEASURE_WEIGHT);
				}
				goto_state(NB_IDLE_POLLING);
			}
			else
				nestbox_nap(NB_LOAD_CELL_POLLING_PERIOD);
		}
		if(nb_state == NB_MEASURE_WEIGHT)
		{
			// Keep LED ON when load cell still in calib mode:
			if(load_cell_get_state() == LOAD_CELL_CALIB)
				HAL_GPIO_WritePin(DEBUG_LED_RTC_CALIB_GPIO_Port, DEBUG_LED_RTC_CALIB_Pin, DEBUG_LED_ON);


			switch(load_cell_process_sample(&nb_evt)){
			case LOAD_CELL_ABOVE_THRESHOLD:
				load_cell_acquire_next_sample();
				calib_inform_weight_measurement_ongoing();
				break;
			case LOAD_CELL_SUFFICIENT_SAMPLES_COLLECTED:
				calib_inform_weight_measurement_completed();
				load_cell_start_polling_mode();
				goto_state(NB_WAIT_END_OF_EVENT);
				if(load_cell_get_state() == LOAD_CELL_CALIB)
				{
					buzzer_on();
					HAL_Delay(100);
					buzzer_off();
				}
				break;
			case LOAD_CELL_BELOW_THRESHOLD:
				calib_inform_weight_removed();
			case LOAD_CELL_EVENT_DURATION_ELAPSED:
				load_cell_start_polling_mode();
				goto_state(NB_PROCESS_AND_SEND);
				break;
			default:
				Error_Handler();
				break;
			}
		}
		if(nb_state == NB_WAIT_END_OF_EVENT)
		{
			HAL_GPIO_WritePin(DEBUG_LED_RTC_CALIB_GPIO_Port, DEBUG_LED_RTC_CALIB_Pin, DEBUG_LED_OFF);

			if(rtc_get_timestamp() - nb_evt.start_timestamp > NB_EVENT_MAX_DURATION)
				goto_state(NB_PROCESS_AND_SEND);
			else if(load_cell_poll() == LOAD_CELL_ABOVE_THRESHOLD)
			{
				nestbox_nap(NB_LOAD_CELL_POLLING_PERIOD);
			}
			else
				goto_state(NB_PROCESS_AND_SEND);
		}
		if(nb_state == NB_PROCESS_AND_SEND)
		{
			nestbox_reset_offset_timer();	// object left perch --> make sure to wait
										// sufficiently long before next offset measurement

			nb_evt.stop_timestamp = rtc_get_timestamp();

			load_cell_process_event(&nb_evt);

			if(load_cell_get_state() == LOAD_CELL_CALIB || nb_evt.rfid_uid == 0)
				comm_send_event(&nb_evt, LOG_TO_MEM_ONLY);
			else
				comm_send_event(&nb_evt, LOG_TO_MEM_AND_TX);


			goto_state(NB_IDLE_POLLING);
		}

		// update USB state before potential sleep states
		if(nestbox_usb_state_update_req)
		{
			system_update_usb_connection_state_blocking();
			__disable_irq();
			nestbox_usb_state_update_req = 0;
			__enable_irq();
		}


		if(nb_state == NB_DAYTIME_SLEEP)
		{
			if(load_cell_get_state() == LOAD_CELL_CALIB) // do not go to daytime sleep if still in calibration mode.
				goto_state(NB_IDLE_POLLING);
			else if(system_get_usb_connection_state() == USB_CONNECTED) // do not go to sleep while USB is connected.
			{
//				goto_state(NB_IDLE_POLLING); // TODO: prod: decide if we want to stay in IDLE mode (which may make USB non-responsive for some time), but lets user add weights
			}
			else if(system_time_to_sleep()) // check again if time is still valid for sleeping (might not be due to USB was connected)
				nestbox_daytime_sleep();
			else
				goto_state(NB_IDLE_POLLING);
		}

		if(nb_state == NB_SHUTDOWN)
		{
			nestbox_shutdown();
		}

		// process cli inputs at every loop iteration.
		while(nestbox_cli_req > 0)
		{
			__disable_irq();
			nestbox_cli_req--;
			__enable_irq();

			cli_process_rx_async();

			HAL_Delay(10);
		}
		if(nestbox_cli_req < 0)
		{
			__disable_irq();
			nestbox_cli_req = 0;
			__enable_irq();

		}

	}
}


