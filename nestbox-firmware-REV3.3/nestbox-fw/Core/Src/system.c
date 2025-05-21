/*
 * system.c
 *
 *  Created on: 16 Sep 2021
 *      Author: raffael
 */

#include "system.h"
#include "rtc.h"
#include "adc.h"
#include "nestbox.h"
#include "usb_device.h"
#include "quadspi.h"
#include "spi.h"
#include "log.h"
#include "cli.h"
#include <usbd_cdc_if.h>
#include "boot.h"
#include "sigfox.h"
#include <math.h>
#include "calibration.h"

system_status_t nb_system = {
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
};
static 	cli_instance_t cliInst; // TODO-OK: not very logical use of this mechanism
SYS_usb_connected_t system_usb_state = USB_CONNECTED;

static int system_shutdown_voltage_limit_reached_counter = 0; // keeps track of VBAT measurements being below the shutdown threshold.


int system_init()
{
	// initialise USB-Serial interface:
	cliInst.cdc_tx = &CDC_Transmit_FS;
	cli_init(&cliInst);

	if(HAL_GPIO_ReadPin(USER_GPIO_GPIO_Port, USER_GPIO_Pin))
	{
		system_usb_state = USB_CONNECTED;
	}
	else
	{
		system_usb_state = USB_DISCONNECTED;
		usb_DeInit(); // USB is not connected --> disable
	}

	nb_system.days_since_boot = 0;
	boot_restore_backup();

	// check if RTC is still valid (thanks to backup VBAT supply from coin cell)
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;

	if(rtc_get_state(&sTime, &sDate) == RTC_UNINIT)
		rtc_reset_date_time(); // the RTC reset puts the clock to a defined value.

	rtc_start_periodic_timer(5);

	return system_measure_status_at_boot(); // get system parameters quietly.
}

// Statistics variables:
#define MONTH_LENGTH	30
#define WEEK_LENGTH		7
uint32_t events_with_id_month[MONTH_LENGTH] = {0,};
uint32_t events_unknown[MONTH_LENGTH] = {0,};
int month_i = 0;

void system_status_fill_statistics(uint32_t* dboot, uint32_t* devt, uint32_t* dowl, uint32_t* wevt, uint32_t* wowl, uint32_t* mevt, uint32_t* mowl)
{
	*dboot = nb_system.days_since_boot;
	*devt = nb_system.events_unknown_id;
	*dowl = nb_system.events_with_id;
	*wevt = 0;
	*wowl = 0;
	*mevt = 0;
	*mowl = 0;

	for(int i = 0; i<MONTH_LENGTH; i++)
	{
		int stat_i = (month_i + i) % MONTH_LENGTH;
		*mowl+=events_with_id_month[stat_i];
		*mevt+=events_unknown[stat_i];
		if(i < WEEK_LENGTH)
		{
			*wowl+=events_with_id_month[stat_i];
			*wevt+=events_unknown[stat_i];
		}
	}
}

void system_status_fill_heartbeat(comm_heartbeat_t* heartbeat, int reset_events_counter)
{
	if(nb_system.events_with_id > UINT8_MAX)
		nb_system.events_with_id = UINT8_MAX;
	if(nb_system.events_unknown_id > UINT8_MAX)
		nb_system.events_unknown_id = UINT8_MAX;

	heartbeat->events_with_id = nb_system.events_with_id;
	heartbeat->events_unknown_id = nb_system.events_unknown_id;
	heartbeat->timestamp = rtc_get_timestamp();
	heartbeat->vbat_mv = nb_system.vbat_mv;
	heartbeat->memory_used = nb_system.memory_used;
	if(nb_system.days_since_boot > UINT8_MAX)
		heartbeat->days_since_boot = UINT8_MAX;
	else
		heartbeat->days_since_boot = nb_system.days_since_boot;

	// set status flags:
	heartbeat->flags = NB_STATUS_FLAGS_MAYOR_FIRMWARE_VERSION;
	if(calib_new_ongoing())
		heartbeat->flags |= NB_STATUS_FLAGS_CALIBRATION_UNFINISHED;
	if(system_usb_state == USB_CONNECTED)
		heartbeat->flags |= NB_STATUS_FLAGS_USB_CONNECTED;
	if(system_usb_state == USB_DETECT_PIN_DISABLED)
		heartbeat->flags |= NB_STATUS_FLAGS_USER_LED_ON;
	if(system_shutdown_voltage_limit_reached_counter > VBAT_SHUTDOWN_THRESHOLD_COUNTER)
		heartbeat->flags |= NB_STATUS_FLAGS_SHUTDOWN;


	if(reset_events_counter)
	{
		// write to local statistics before clearing variables:
		events_with_id_month[month_i] = nb_system.events_with_id;
		month_i = (month_i+1) % MONTH_LENGTH;

		nb_system.events_with_id = 0;
		nb_system.events_unknown_id = 0;
	}
}

void system_increment_events_with_id()
{
	nb_system.events_with_id++;
}

void system_increment_events_unknown_id()
{
	nb_system.events_unknown_id++;
}

int system_info_invalid()
{
	return SYSTEM_OK;
	// todo-ok handle non-valid downlink

	return SYSTEM_INFO_INVALID;
}

int system_measure_status_at_boot()
{
	for(int i=0; i<VBAT_SHUTDOWN_THRESHOLD_COUNTER+1; i++)
	{
		// this needs to run at least VBAT_SHUTDOWN_THRESHOLD_COUNTER times to trigger a SYSTEM_POWER_DOWN response:
		system_measure_status(VBAT_STARTUP_MIN_MV);

		HAL_Delay(20);
	}

	return system_measure_status(VBAT_STARTUP_MIN_MV);
}

int system_measure_status(unsigned int vbat_limit)
{
	// get memory status:
	nb_system.memory_used = round(log_get_percent_full());


	// measure battery voltage:
	nb_system.soc = io_battery_level();
	nb_system.vbat_mv = io_get_last_battery_voltage();


	if(nb_system.vbat_mv < vbat_limit)
	{
		system_shutdown_voltage_limit_reached_counter++;
		if(system_shutdown_voltage_limit_reached_counter > VBAT_SHUTDOWN_THRESHOLD_COUNTER)
		{
			// re-measure to be sure:
			nb_system.soc = io_battery_level();
			nb_system.vbat_mv = io_get_last_battery_voltage();

			if(nb_system.vbat_mv < vbat_limit)
				return SYSTEM_POWER_DOWN;
			else
				system_shutdown_voltage_limit_reached_counter--;

		}
	}
	else
	{
		system_shutdown_voltage_limit_reached_counter = 0;
	}

	return SYSTEM_OK;
}

void system_update_usb_connection_state_blocking()
{
	HAL_Delay(100); // wait for eventual de-bouncing.
	if(system_usb_state < USB_DETECT_PIN_DISABLED)
	{
#ifdef NB2_EMC_TEST_MODE
		// stay in USB connected mode in order to stay in EMC test modes after unplugging laptop
		if(system_usb_state == USB_DISCONNECTED)
		{
			system_usb_state = USB_CONNECTED;
			MX_USB_DEVICE_Init();

		}
#else
		if(HAL_GPIO_ReadPin(USER_GPIO_GPIO_Port, USER_GPIO_Pin) && system_usb_state == USB_DISCONNECTED)
		{
			system_usb_state = USB_CONNECTED;
			MX_USB_DEVICE_Init();

		}
		else if(HAL_GPIO_ReadPin(USER_GPIO_GPIO_Port, USER_GPIO_Pin)==0 && system_usb_state == USB_CONNECTED)
		{
			system_usb_state = USB_DISCONNECTED;
			usb_DeInit(); // USB is not connected --> disable
		}
#endif
	}
}

SYS_usb_connected_t system_get_usb_connection_state()
{
	return system_usb_state;
}

int system_disable_usb_connection_state_pin()
{
	if(system_usb_state == USB_DISCONNECTED)
	{
		system_usb_state = USB_DETECT_PIN_DISABLED;
		return 1;
	}

	return 0;
}

void system_enable_usb_connection_state_pin()
{
	if(system_usb_state == USB_DETECT_PIN_DISABLED)
	{
		system_usb_state = USB_DISCONNECTED;
	}
	system_update_usb_connection_state_blocking();
}

static int system_woke_up_too_early = 0;


/* returns 1 if the system should go to sleep at the current time of the day */
int system_time_to_sleep()
{
	uint8_t Year;
	uint8_t Month;
	uint8_t Date;
	uint8_t Hours;
	uint8_t Minutes;
	uint8_t Seconds;

	if(rtc_get_utc(&Year, &Month, &Date, &Hours, &Minutes, &Seconds) == HAL_ERROR)
		return 0;

	if(Hours >= NB_DAYTIME_POWER_OFF_HOUR_UTC && Hours < NB_DAYTIME_POWER_ON_HOUR_UTC)
	{
		return 1;
	}
	else
	{
		system_woke_up_too_early = 0; // reset this flag to signal that a sigfox status message can be sent when going to sleep again.
		return 0;
	}
}


/***************** Power saving functions **********************/


void system_daytime_sleep()
{
	// stop periodic wake up
	rtc_stop_periodic_timer();

	// instead set a defined alarm to wake up the system in the evening:
	rtc_set_alarm(NB_DAYTIME_POWER_ON_HOUR_UTC, 0);

	// send a status message, if we woke up for a good reason:
	if(system_woke_up_too_early == 0)
	{
		// we ensure that the status message before going to sleep can be sent, by resetting the message counter once per day here.
		sigfox_reset_daily_message_counter();
		comm_send_status(COMM_UPLINK_ONLY, LOG_TO_MEM_AND_TX);
	}

	// backup state and logs:
	boot_create_backup();

	// turn off peripherals power supply:
	HAL_GPIO_WritePin(BAT_TEST_EN_GPIO_Port, BAT_TEST_EN_Pin, 0);
	HAL_GPIO_WritePin(DEBUG_LED_RTC_CALIB_GPIO_Port, DEBUG_LED_RTC_CALIB_Pin, DEBUG_LED_OFF);
	HAL_GPIO_WritePin(RFID_SUPPLY_EN_GPIO_Port, RFID_SUPPLY_EN_Pin, 0);

	// enable pull-ups and pull-downs:
	HAL_PWREx_EnableGPIOPullUp(PWR_LOAD_CELL_LDO_EN_GPIO_Port, LOAD_CELL_LDO_EN_Pin); // we leave the LDO on because we measured a lower power consumption if VAA did not drop below 1V. (todo-next: TEST IF HIGHER POWER IS DUE TO AC-EXCITATION MOSFETS STILL BEING DRIVEN HIGH)
	HAL_PWREx_EnableGPIOPullDown(PWR_BAT_TEST_EN_GPIO_Port, BAT_TEST_EN_Pin);
	// flash and 5V already have external pull-up/down resistor

	comm_module_off();
	nb_flash_power_off();
// qspi_DeInit(); // already done in nb_flash_power_off

	// Stop periodic timer:
	rtc_stop_periodic_timer();
	HAL_SuspendTick();
//	tim_DeInit(); clock is already suspended --> timer de-init has no influence.
	adc_DeInit();
	spi_DeInit();

//	usb_DeInit();

	//configure pull-down state:
	HAL_PWREx_EnablePullUpPullDownConfig();

//	HAL_PWR_EnterSTANDBYMode(); 	// power consumption: sleep > stop > standby
//	HAL_PWREx_EnterSHUTDOWNMode();
	HAL_PWREx_EnterSTOP2Mode(PWR_SLEEPENTRY_WFI);

	// resume system:
	system_restore_clock();
	//	MX_USB_DEVICE_Init();

	HAL_ResumeTick();

	MX_ADC1_Init();
//	MX_TIM16_Init();
//	MX_USB_DEVICE_Init();
//	MX_CRC_Init();
//	MX_TIM1_Init();
	MX_SPI3_Init();
//	MX_USART3_UART_Init(); // is inside comm_module_on()

	// check time and determine if wake up was too early:
	if(system_time_to_sleep())
	{
		system_woke_up_too_early = 1;
	}
	else
	{
		nb_system.days_since_boot++;
	}

}


void system_shutdown()
{
	// backup state and logs:
	boot_create_backup();

	// turn off peripherals power supply:
	HAL_GPIO_WritePin(FLASH_POWER_ON_N_GPIO_Port, FLASH_POWER_ON_N_Pin, 1);
	HAL_GPIO_WritePin(BAT_TEST_EN_GPIO_Port, BAT_TEST_EN_Pin, 0);
	HAL_GPIO_WritePin(DEBUG_LED_RTC_CALIB_GPIO_Port, DEBUG_LED_RTC_CALIB_Pin, DEBUG_LED_OFF);
	HAL_GPIO_WritePin(RFID_SUPPLY_EN_GPIO_Port, RFID_SUPPLY_EN_Pin, 0);
	HAL_GPIO_WritePin(LOAD_CELL_LDO_EN_GPIO_Port, LOAD_CELL_LDO_EN_Pin, 0);

	// enable pull-ups and pull-downs:
	HAL_PWREx_EnableGPIOPullUp(PWR_LOAD_CELL_LDO_EN_GPIO_Port, LOAD_CELL_LDO_EN_Pin); // we leave the LDO on because we measured a lower power consumption if VAA did not drop below 1V. (todo-next: TEST IF HIGHER POWER IS DUE TO AC-EXCITATION MOSFETS STILL BEING DRIVEN HIGH)
	HAL_PWREx_EnableGPIOPullDown(PWR_BAT_TEST_EN_GPIO_Port, BAT_TEST_EN_Pin);
	// flash and 5V already have external pull-up/down resistor

	comm_module_off();

	nb_flash_power_off();
// qspi_DeInit(); // already done in nb_flash_power_off

	// Stop periodic timer:
	rtc_stop_periodic_timer();
	HAL_SuspendTick();
//	tim_DeInit(); clock is already suspended --> timer de-init has no influence.
	adc_DeInit();
//	usb_DeInit();

	//configure pull-down state:
	HAL_PWREx_EnablePullUpPullDownConfig();

//	HAL_PWR_EnterSTANDBYMode(); 	// power consumption: sleep > stop > standby
	HAL_PWREx_EnterSHUTDOWNMode();
//	HAL_PWREx_EnterSTOP2Mode(PWR_SLEEPENTRY_WFI);
}

void system_sleep_IT(unsigned int seconds)
{
	if(system_get_usb_connection_state() == USB_DISCONNECTED)
	{
		rtc_stop_periodic_timer();
		rtc_start_periodic_timer(seconds);

		// De-initialise MCU peripherals in reverse order from their initialising calls:
	//	tim_DeInit(); clock is already suspended --> timer de-init has no influence.
	//	uart_DeInit(); --> done at sigfox low power configuration.
		
		nb_flash_power_off();
		// qspi_DeInit(); // already done in nb_flash_power_off		spi_DeInit();

		// go into lowest power mode that only wakes up with RTC clock:ff
		adc_DeInit();
		HAL_SuspendTick();
	//	usb_DeInit();

		// enter STOP mode, as there is no need to respond to CLI inputs
		HAL_PWREx_EnterSTOP2Mode(PWR_SLEEPENTRY_WFI);
		system_restore_clock();
	//	MX_USB_DEVICE_Init();

		HAL_ResumeTick();
		MX_ADC1_Init();

	//	MX_DMA_Init();
		MX_SPI3_Init();
	//	nb_flash_power_on_blocking(); // this is done directly inside qspi commands if needed.
	//	MX_USART2_UART_Init();
	//	MX_TIM16_Init();
	//	MX_TIM2_Init();
	}
	else
	{
		// short delay in order to react to CLI inputs.
		HAL_Delay(20);
	}

}
