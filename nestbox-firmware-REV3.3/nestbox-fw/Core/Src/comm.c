/*
 * comm.c
 *
 *  Created on: 15 Sep 2021
 *      Author: raffael
 */

#include "system.h"
#include "comm.h"
#include "sigfox.h"
#include "main.h"
#include <math.h>
#include "load_cell.h"
#include "rtc.h"
#include "log.h"

static enum commState {
	COMM_STATE_UNINIT = 0,
	COMM_STATE_ON,
	COMM_STATE_SLEEP,
	COMM_STATE_OFF,
	COMM_STATE_ERROR,
} comm_state = COMM_STATE_UNINIT;

static enum downlinkState {
	COMM_DOWNLINK_NOATTEMPTS = 0,
	COMM_DOWNLINK_SUCCESS,
	COMM_DOWNLINK_FAILED,
} comm_downlink_state = COMM_DOWNLINK_NOATTEMPTS;

uint32_t comm_downlink_last_success_time = 0;

void comm_module_sleep()
{
	comm_module_off(); // currently, we directly switch module off.

//	// todo-ok: put to deep sleep & wake up with GPIO9
//
//	// currently, we put to sleep mode & wake up with uart:
//	sigfox_module_sleep();
//
//	comm_state = COMM_STATE_SLEEP;
}

// keep track of power-off time of the module, and keep it off for at least 5 seconds:
unsigned int comm_last_power_off_time = 0;

int comm_module_on()
{
	if(comm_state == COMM_STATE_ON)
		return COMM_OK;
	if(comm_state == COMM_STATE_OFF)
	{
		while(rtc_get_timestamp() - comm_last_power_off_time < SIGFOX_MIN_OFF_TIME_SECONDS)
		{
			HAL_Delay(1000);
		}
		HAL_GPIO_WritePin(WIFI_EN_GPIO_Port, WIFI_EN_Pin, 1);
		HAL_Delay(400); // empirically tested this needed wakeup time (200ms). doubled to 400ms as a safety margin.
		comm_state = COMM_STATE_SLEEP;
	}

	if(comm_state == COMM_STATE_SLEEP)
	{
		sigfox_wakeup_module();
		comm_state = COMM_STATE_ON;

		return COMM_OK;
	}

	return COMM_ERROR;
}

void comm_module_off()
{
	HAL_Delay(50); // empirically tested this needed sleep time

	sigfox_module_sleep(); // also de-inits uart.

	HAL_GPIO_WritePin(WIFI_EN_GPIO_Port, WIFI_EN_Pin, 0);
	HAL_Delay(50); // empirically tested this needed sleep time

	comm_last_power_off_time = rtc_get_timestamp();

	if(comm_state != COMM_STATE_ERROR)
		comm_state = COMM_STATE_OFF;
}

void comm_init()
{
	comm_module_off();
	HAL_Delay(100);
	comm_module_on();

	comm_state = COMM_STATE_UNINIT;
	// Initialise SIGFOX module:
#ifdef DEBUG_WITHOUT_COMM_MODULE
	comm_state = COMM_STATE_ON;

#else
	if(sigfox_init(HAL_Delay) == SIGFOX_OK)
	{
		comm_state = COMM_STATE_ON;
//		comm_module_sleep();
	}
	else
	{
		comm_state = COMM_STATE_ERROR;
		comm_module_off();
	}
#endif
}

static comm_heartbeat_t comm_hb = {
		COMM_HEADER_HEARTBEAT,
		0,
		0,
		0,
		0,
		0,
		0,
		0
};

static comm_offset_t comm_of = {
		COMM_HEADER_OFFSET,
		0,
		0,
		0,
		0,
		0,
		0,
		0
};

static comm_calib_t comm_clb = {
		COMM_HEADER_CALIB,

};

static comm_event_t comm_evt;
static int comm_force_tx_while_usb_connected = 0;

void comm_force_tx_once()
{
	comm_force_tx_while_usb_connected = 1; // to allow a cli command to tx a sigfox message.
}

static int comm_tx_frame(uint8_t* bytes, unsigned int len, int request_downlink, int log_only)
{
	// backup message to memory:
	log_write_message(bytes, len);

	int retval = COMM_ERROR;

	// do not make sigfox transmissions while USB is connected.
	if(log_only == 0 &&
			(comm_force_tx_while_usb_connected || system_get_usb_connection_state() != USB_CONNECTED)
			&& comm_module_on() != COMM_ERROR)
	{
		comm_force_tx_while_usb_connected = 0;

		retval = sigfox_tx_frame(bytes, len, request_downlink);

		if(!request_downlink)
			comm_module_sleep();
	}

	if(retval == SIGFOX_OK)
		return COMM_OK;
	else
		return COMM_ERROR;
}

void comm_wakeup_get_time_from_basestation()
{
	if(comm_send_status(COMM_REQUEST_ANSWER, LOG_TO_MEM_AND_TX) != COMM_ERROR)
	{
		uint8_t downlink_bytes[8];
		if(sigfox_rx_downlink(downlink_bytes, SIGFOX_DOWNLINK_TIMEOUT) == SIGFOX_OK)
		{
			// todo-next: create struct once message is redefined and sent from control portal
			uint32_t timestamp = downlink_bytes[4];
			timestamp = (timestamp<<8) + downlink_bytes[5];
			timestamp = (timestamp<<8) + downlink_bytes[6];
			timestamp = (timestamp<<8) + downlink_bytes[7];

			rtc_set_timestamp(timestamp + SIGFOX_DOWNLINK_TIMESTAMP_DELAY_SECONDS);

			// report if downlink was successful:
			comm_downlink_state = COMM_DOWNLINK_SUCCESS;
			comm_downlink_last_success_time = timestamp;

			// todo-next: extract other settings from downlink message
		}
		else
		{
			comm_downlink_state = COMM_DOWNLINK_FAILED;
		}

		// delay to wait for OOB message to be sent. 
		HAL_Delay(SIGFOX_OOB_DELAY_MS);

		// (re-)send heartbeat with updated time.
		comm_send_status(COMM_UPLINK_ONLY, LOG_TO_MEM_AND_TX);

		comm_module_sleep();
	}
}

int comm_send_status(int request_downlink, int log_only)
{
	comm_hb.header = COMM_HEADER_HEARTBEAT;

	// Only reset events counter if message is uploaded
	int reset_events_counter = 1;
	if(log_only)
		reset_events_counter = 0;

	system_status_fill_heartbeat(&comm_hb, reset_events_counter);

	return comm_tx_frame((uint8_t*)&comm_hb,sizeof(comm_hb), request_downlink, log_only);
}

void comm_send_offset(int log_only)
{

	comm_of.header = COMM_HEADER_OFFSET;
	load_cell_get_offset(&comm_of);

#ifdef NB_CONSTANT_CALIBRATION_MODE
	comm_tx_frame((uint8_t*)&comm_of, sizeof(comm_of), SIGFOX_UPLINK_ONLY, LOG_TO_MEM_ONLY);
#else
	comm_tx_frame((uint8_t*)&comm_of, sizeof(comm_of), SIGFOX_UPLINK_ONLY, log_only);
#endif
}

void comm_send_calibration()
{
	comm_clb.header = COMM_HEADER_CALIB;
	calib_pack_message(&comm_clb);

#ifdef NB_CONSTANT_CALIBRATION_MODE
	comm_tx_frame((uint8_t*)&comm_clb, sizeof(comm_clb), SIGFOX_UPLINK_ONLY, LOG_TO_MEM_ONLY); // in constant calib mode, do not tx sigfox.
#else
	comm_tx_frame((uint8_t*)&comm_clb, sizeof(comm_clb), SIGFOX_UPLINK_ONLY, LOG_TO_MEM_AND_TX);
#endif
}

void comm_send_event(nestbox_event_t* evt, int log_only)
{
	comm_evt.duration = evt->stop_timestamp - evt->start_timestamp;
	if(comm_evt.duration > NB_EVENT_MAX_DURATION)
		comm_evt.duration = NB_EVENT_MAX_DURATION;

	comm_evt.flags = evt->flags;

	comm_evt.weight = round(evt->weight_estimate * 100.0);
	comm_evt.uid = (uint32_t)evt->rfid_uid;
	comm_evt.raw_adc_pos = (evt->ac_pos_median) >> NB_RAW_VALUE_BIT_SHIFT;
	comm_evt.stddev = round(evt->stddev_estimate * 10.0);
	comm_evt.temperature = round(evt->temperature);

#ifdef NB_CONSTANT_CALIBRATION_MODE
	comm_tx_frame((uint8_t*)&comm_evt, sizeof(comm_evt), SIGFOX_UPLINK_ONLY, LOG_TO_MEM_ONLY);
#else
	comm_tx_frame((uint8_t*)&comm_evt, sizeof(comm_evt), SIGFOX_UPLINK_ONLY, log_only);
#endif
}

// Functions called by CLI:
int comm_get_info_string(char* buf, int buf_len)
{
	if(comm_module_on() != COMM_ERROR)
	{
		int buf_pos = snprintf(buf, buf_len, "sigfox,");
		int id_len = sigfox_get_device_id(&buf[buf_pos], buf_len - buf_pos);
		if(id_len == 0)
		{
			//comm_module_off(); // turning module off will delay subsequent sigfox transactions
			return COMM_ERROR;
		}

		buf_pos += id_len;

		buf[buf_pos] = ',';

		buf_pos+=1;

		if(sigfox_get_pac(&buf[buf_pos], buf_len - buf_pos))
		{
			return COMM_OK;
//			comm_module_off();	// turning module off will delay subsequent sigfox transactions
		}
	}

	return COMM_ERROR;
}

int comm_get_downlink_string(char* buf, int buf_len)
{
	return snprintf(buf, buf_len, "sigstat,commstate=%u, downlink=%u, last=%lu", comm_state, comm_downlink_state, comm_downlink_last_success_time);
}

void comm_emi_test_cw_on(uint32_t freq)
{
	if(comm_module_on() != COMM_ERROR)
	{
		sigfox_emi_cw_on(freq);
	}
}

void comm_emi_test_pattern_on(uint32_t freq)
{
	if(comm_module_on() != COMM_ERROR)
	{
		sigfox_emi_test_pattern_on(freq);
	}
}

void comm_emi_test_off()
{
	comm_module_off();
}
