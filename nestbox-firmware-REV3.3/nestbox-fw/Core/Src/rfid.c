/*
 * rfid.c
 *
 *  Created on: 29 Jun 2021
 *      Author: raffael
 */


/*
 * rfid_reader.c
 *
 *  Created on: 27 Mar 2017
 *      Author: raffael
 */

#include "rfid.h"
#include "EM4095.h"
#include "comm_types.h" // for flags
#include <tim.h>

/*************** LF: **********************/
static em4095_t rfid_def;
static tagdata lf_tagdata;

#define RFID_N_ID_COMPARISONS	3
static int rfid_tag_detected = 0;
static uint64_t rfid_id_control[RFID_N_ID_COMPARISONS];

void rfid_start_detection();
void rfid_stop_detection();
void rfid_reset_detection_counts();




int rfid_detect_id()
{
	/* This function does:
	 *
	 * - turn on rfid reader
	 * - attempt to read tag (until timeout)
	 * - turn off reader
	 * - update lf_tagdata.tagId
	 *
	 * - return RFID_TIMEOUT_OR_FAILED 	if failed
	 * - return RFID_SUCCESS			if tag could be read
	 *
	 */

	rfid_start_detection();

	uint32_t timeout = HAL_GetTick();

	while (1) {
		while(rfid_tag_detected == 0)
		{
			if(HAL_GetTick() - timeout > RFID_READER_ON_TIME_MS)
			{
				rfid_stop_detection();
				return RFID_TIMEOUT_OR_FAILED;
			}
		}

		// tag has been successfully read out.
		rfid_tag_detected = 0;

		if(em4100_format(&rfid_def, &lf_tagdata) == EM4095_OK)
		{
			rfid_id_control[lf_tagdata.detection_counts] = lf_tagdata.tagId;

			lf_tagdata.detection_counts += 1;

			lf_tagdata.valid = 1;

			if(lf_tagdata.detection_counts >= RFID_N_ID_COMPARISONS)
			{
				// check if all 3 readings were identical, only then the tag is deemed valid.
				int i=0;
				int ids_identical = 1;
				for(i=1; i<RFID_N_ID_COMPARISONS; i++)
				{
					if(rfid_id_control[0] != rfid_id_control[i])
					{
						ids_identical = 0;
					}
				}
				if(ids_identical)
				{
					rfid_stop_detection();
					return RFID_SUCCESS;
				}
				else
				{
					rfid_reset_detection_counts();
				}
			}
		}

		if(lf_tagdata.valid)
		{
//		    log_write_new_rfid_entry(lf_tagdata.tagId);
		}
		else
		{
			rfid_reset_detection_counts();
		}
	}
}



void rfid_test()
{
	HAL_GPIO_WritePin(DEBUG_LED_RTC_CALIB_GPIO_Port, DEBUG_LED_RTC_CALIB_Pin, 1);
	rfid_start_detection();

    while (1) {
		while(rfid_tag_detected == 0);
		// tag has been successfully read out.
		rfid_tag_detected = 0;

		if(em4100_format(&rfid_def, &lf_tagdata) == EM4095_OK)
		{
			lf_tagdata.detection_counts += 1;

			lf_tagdata.valid = 1;

			if(lf_tagdata.detection_counts>2)
			{
				rfid_stop_detection();
			}
		}

		if(lf_tagdata.valid)
		{
//		    log_write_new_rfid_entry(lf_tagdata.tagId);

			rfid_reset_detection_counts();
			HAL_GPIO_WritePin(DEBUG_LED_RTC_CALIB_GPIO_Port, DEBUG_LED_RTC_CALIB_Pin, 0);
			HAL_Delay(2000);

			HAL_GPIO_WritePin(DEBUG_LED_RTC_CALIB_GPIO_Port, DEBUG_LED_RTC_CALIB_Pin, 1);
			rfid_start_detection();
		}
		else
		{
			rfid_reset_detection_counts();
		}
    }
}

int rfid_get_compressed_id(nestbox_event_t* evt)
{
	// IMPORTANT NOTE: It is assumed that all bird rings have the format (hex)XX00XXXXXX

	if(lf_tagdata.tagId & 0x00FF000000)
	{
		evt->flags |= NB_FLAGS_UNKNOWN_TAG_ID;

		// in this case, we transmit the 32 least significant bits without Customer ID byte.
		evt->rfid_uid = (uint32_t)lf_tagdata.tagId;
	}
	else
	{
		evt->rfid_uid = 0x0000FFFFFF & lf_tagdata.tagId;
		evt->rfid_uid |= (0xFF000000 & (lf_tagdata.tagId >> 8));
	}

    return 0;
}

void rfid_on(){
	// turn on 5V supply:
	HAL_GPIO_WritePin(RFID_SUPPLY_EN_GPIO_Port, RFID_SUPPLY_EN_Pin, 1);

	// turn on RF field:
	HAL_GPIO_WritePin(RFID_SHDN_GPIO_Port, RFID_SHDN_Pin, 1);
}

void rfid_off(){
	// turn off RF field:
	HAL_GPIO_WritePin(RFID_SHDN_GPIO_Port, RFID_SHDN_Pin, 0);

	// turn off 5V supply:
	HAL_GPIO_WritePin(RFID_SUPPLY_EN_GPIO_Port, RFID_SUPPLY_EN_Pin, 0);
}

void rfid_start_detection()
{
	rfid_reset_detection_counts();

	lf_tagdata.valid = 0;

	rfid_on();

	// start timer & enable capture interrupt.
	rfid_tim_start();

}

void rfid_reset_detection_counts()
{
    lf_tagdata.detection_counts = 0;
    int i;
    for (i=0; i<RFID_N_ID_COMPARISONS; i++)
    {
    	rfid_id_control[i] = 0;
    }
}

void rfid_stop_detection()
{
	// stop timer & enable capture interrupt.
	rfid_tim_stop();

	rfid_off();
}

volatile uint8_t last_bit = 0;
volatile uint16_t last_timer_val = 0;
static uint16_t timediff = 0;

//one bit period = 500us = 62.5 cycles = shortest interval
//mid interval = 750us = 94 cycles
//longest interval = 1000us = 125 cycles

const uint16_t threshold_short = 78;
const uint16_t threshold_long = 109;

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3) // RFID data in

	{
		// for EM reader, this is the data pin with CCR!!!
		timediff = htim->Instance->CCR3 - last_timer_val;
		last_timer_val = htim->Instance->CCR3;

		if(timediff > 2000)
		{
			timediff = 0xFFFF - timediff;
		}
		else if(timediff > threshold_long)
		{//01
			//this is for sure a one preceded by a zero
			last_bit = 1;
			if(em4095_read(&rfid_def, 0)==EM4095_DATA_OK)
				rfid_tag_detected = 1;
			if(em4095_read(&rfid_def, 1)==EM4095_DATA_OK)
				rfid_tag_detected = 1;
		}
		else if(timediff < threshold_short)
		{
			if(last_bit == 1)
			{//1
				if(em4095_read(&rfid_def, 1)==EM4095_DATA_OK)
					rfid_tag_detected = 1;
			}
			else
			{//0
				if(em4095_read(&rfid_def, 0)==EM4095_DATA_OK)
					rfid_tag_detected = 1;
			}
		}
		else
		{
			if(last_bit == 1)
			{//00
				if(em4095_read(&rfid_def, 0)==EM4095_DATA_OK)
					rfid_tag_detected = 1;
				if(em4095_read(&rfid_def, 0)==EM4095_DATA_OK)
					rfid_tag_detected = 1;
				last_bit = 0;
			}
			else
			{//(0)1
				if(em4095_read(&rfid_def, 1)==EM4095_DATA_OK)
					rfid_tag_detected = 1;
				last_bit = 1;
			}

		}
	}
}


// Functions called by CLI:
int rfid_get_string(char* buf, int buf_len)
{


	if(rfid_detect_id() == RFID_TIMEOUT_OR_FAILED)
	{
		return snprintf(buf, buf_len, "rfid, 0");
	}
	else
	{
		nestbox_event_t tmp_evt;
		rfid_get_compressed_id(&tmp_evt);
		return snprintf(buf, buf_len, "rfid, %lx", (uint32_t)tmp_evt.rfid_uid);
	}
}

