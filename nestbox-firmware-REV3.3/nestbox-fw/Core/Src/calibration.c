/*
 * calibration.c
 *
 *  Created on: 1 Oct 2021
 *      Author: raffael.tschui
 */

#include "calibration.h"
#include <stddef.h>
#include <math.h>
#include <string.h>
#include "rtc.h"
#include "../statistics/statistics.h"
#include "log.h"
#include "load_cell.h"

static load_cell_calib_t calib;

const float calib_weights[NB_N_CALIBRATION_WEIGHTS] = NB_DEFAULT_CALIBRATION_WEIGHTS;
const float calib_tolerances[NB_N_CALIBRATION_WEIGHTS] = NB_CALIBRATION_WEIGHTS_TOLERANCES;
static nestbox_event_t calib_events[NB_N_CALIBRATION_WEIGHTS];
static char calib_events_available[NB_N_CALIBRATION_WEIGHTS] = {0,0,0,0};

static unsigned char calib_event_state = CALIB_NO_WEIGHT_ON_PERCH;

static void calib_apply(float new_slope, int32_t new_intercept, float r2, float temperature, struct calib_info_struct info);

float calib_get_slope(){
	return calib.slope;
}

int32_t calib_get_detection_threshold()
{
	return calib.detection_threshold;
}

int32_t calib_get_stdev_max_valid()
{
	return calib.stdev_max_valid;
}

int32_t calib_get_max_sample_diff_valid()
{
	return calib.sample_diff_max_valid;
}


uint8_t calib_get_id()
{
	return calib.info.id;
}

int calib_add_event(nestbox_event_t* evt)
{
	// identify weight:

	for(int i=0; i<NB_N_CALIBRATION_WEIGHTS; i++)
	{

		if(calib_weights[i] - calib_tolerances[i] < evt->weight_estimate &&
				calib_weights[i] + calib_tolerances[i] > evt->weight_estimate)
		{
			// the weight was identified --> store it:
			memcpy(&calib_events[i], evt, sizeof(calib_events[i]));
			calib_events_available[i] = 1;
			return CALIB_OK;
		}
	}

	return CALIB_ERROR;
}

int calib_new_ongoing()
{
	int ongoing = 0;

	if(calib_events_available[0] > 0)
		ongoing = 1;
	if(calib_events_available[1] > 0)
		ongoing = 1;

	#if(NB_N_CALIBRATION_WEIGHTS > 2)
		if(calib_events_available[2] > 0)
			ongoing = 1;
	#endif
	#if(NB_N_CALIBRATION_WEIGHTS > 3)
		if(calib_events_available[3] > 0)
			ongoing = 1;
	#endif


	return ongoing;
}

void calib_reset_ongoing()
{
	calib_events_available[0] = 0;
	calib_events_available[1] = 0;
#if(NB_N_CALIBRATION_WEIGHTS > 2)
	calib_events_available[2] = 0;
#endif
#if(NB_N_CALIBRATION_WEIGHTS > 3)
	calib_events_available[3] = 0;
#endif
}

void calib_inform_weight_removed()
{
	calib_event_state = CALIB_NO_WEIGHT_ON_PERCH;
}

void calib_inform_weight_detected()
{
	calib_event_state = CALIB_WEIGHT_DETECTED; // before RFID detection (if not in calib mode)
}

void calib_inform_weight_measurement_ongoing()
{
	calib_event_state = CALIB_WEIGHT_MEASUREMENT_ONGOING; // after RFID detection (if not in calib mode)
}

void calib_inform_weight_measurement_completed()
{
	calib_event_state = CALIB_WEIGHT_MEASUREMENT_COMPLETED;
}



// Conclude calibration:
int calib_process_events()
{
	double x[NB_N_CALIBRATION_WEIGHTS];
	double y[NB_N_CALIBRATION_WEIGHTS];
	float average_temperature = 0.0;

	int n_events = 0;

	for(int i=0; i<NB_N_CALIBRATION_WEIGHTS; i++)
	{
		if(calib_events_available[i] == 1)
		{
			x[n_events] = calib_weights[i];
			y[n_events] = calib_events[i].raw_median;
			average_temperature += calib_events[i].temperature;
			n_events++;
		}
	}

	if(n_events < NB_N_MINIMUM_CALIBRATION_WEIGHTS)
	{
		return CALIB_ERROR;
	}


	average_temperature = average_temperature/n_events;

	// gather calibration info:
	struct calib_info_struct new_calib_info;
	new_calib_info.id = (calib.info.id + 1) % COMM_N_CALIB_IDS;
	new_calib_info.used_w1 = (calib_events_available[0] > 0);
	new_calib_info.used_w2 = (calib_events_available[1] > 0);
#if(NB_N_CALIBRATION_WEIGHTS > 2)
	new_calib_info.used_w3 = (calib_events_available[2] > 0);
#else
	new_calib_info.used_w3 = 0;
#endif
#if(NB_N_CALIBRATION_WEIGHTS > 3)
	new_calib_info.used_w4 = (calib_events_available[3] > 0);
#else
	new_calib_info.used_w4 = 0;
#endif

	// compute linear regression:
	double new_slope, new_intercept, r2;

	r2 = linear_regression(x, y, n_events, &new_slope, &new_intercept);

	if(r2 > NB_LINREG_MIN_R2 && r2<1.0)
	{
		calib_apply(new_slope, round(new_intercept), r2, average_temperature, new_calib_info);

		unsigned int i = 0;
		for(i = 0; i<NB_N_CALIBRATION_WEIGHTS; i++)
		{
			calib_events_available[i] = 0;
		}
		return CALIB_OK;
	}

	return CALIB_ERROR;
}


//void linregtest()
//{
//	double x[NB_N_CALIBRATION_WEIGHTS];
//	double y[NB_N_CALIBRATION_WEIGHTS];
//
//	int n_events = 0;
//
//	for(int i=0; i<NB_N_CALIBRATION_WEIGHTS; i++)
//	{
//			x[n_events] = i;
//			y[n_events] = i*3+4;
//			n_events++;
//	}
//
//	x[0] += 0.1;
//
//	// compute linear regression:
//
//	double new_slope, new_intercept, r2;
//
//	r2 = linear_regression(x, y, n_events, &new_slope, &new_intercept);
//
//	if(r2 > NB_LINREG_MIN_R2 && r2<1.0)
//	{
//		struct calib_info_struct a;
//		calib_apply(new_slope, round(new_intercept), r2, 2, a);
//	}
//}


static void calib_apply(float new_slope, int32_t new_intercept, float r2, float temperature, struct calib_info_struct info)
{
	calib.timestamp = rtc_get_timestamp();
	calib.slope = new_slope;
	calib.intercept = new_intercept;
	calib.r2 = r2;
	calib.temperature = temperature;

	calib.detection_threshold = new_slope * NB_DETECTION_THRESHOLD;
	calib.overload_threshold = new_slope * NB_OVERLOAD_THRESHOLD;
	calib.stdev_max_valid = new_slope * NB_MAX_STDEV_FOR_VALID_SAMPLE;
	calib.sample_diff_max_valid = new_slope * NB_MAX_DIFF_FOR_VALID_SAMPLE;

	calib.info = info;
}

int calib_pack_message(comm_calib_t* clb)
{
	clb->info = calib.info;
	clb->slope = round(calib.slope * 100.0);
	clb->intercept = calib.intercept >> NB_RAW_VALUE_BIT_SHIFT; // shifted by 5 bits
	clb->temperature = round(calib.temperature*100.0);
	clb->r2 = calib.r2;

	return 0;
}

void calib_backup()
{
	GD5F1GQ4_write_backup_register(LOG_BKP_REG_CALIB_SLOPE, *(uint32_t*)(&calib.slope));
	GD5F1GQ4_write_backup_register(LOG_BKP_REG_CALIB_INTERCEPT, calib.intercept);
	GD5F1GQ4_write_backup_register(LOG_BKP_REG_CALIB_R2, *(uint32_t*)&calib.r2);
	GD5F1GQ4_write_backup_register(LOG_BKP_REG_CALIB_TEMP, *(uint32_t*)&calib.temperature);
	GD5F1GQ4_write_backup_register(LOG_BKP_REG_CALIB_INFO, *((uint8_t*)&(calib.info)));
	GD5F1GQ4_write_backup_register(LOG_BKP_REG_CALIB_TIME, (uint32_t)calib.timestamp);
	GD5F1GQ4_write_backup_register(LOG_BKP_REG_CALIB_PW, (uint32_t)CALIB_BACKUP_PW);

}

void calib_restore()
{
	float new_slope;
	int32_t new_intercept;
	float r2;
	float temperature;
	uint8_t info_byte;
	struct calib_info_struct info;
	uint32_t timestamp;
	uint32_t pw = GD5F1GQ4_read_backup_register(LOG_BKP_REG_CALIB_PW);

	// todo-next: compare CRC

	if(pw ==CALIB_BACKUP_PW)
	{
		uint32_t tmpval = GD5F1GQ4_read_backup_register(LOG_BKP_REG_CALIB_SLOPE);
		new_slope = *(float*)&tmpval;

		new_intercept = GD5F1GQ4_read_backup_register(LOG_BKP_REG_CALIB_INTERCEPT);

		tmpval = GD5F1GQ4_read_backup_register(LOG_BKP_REG_CALIB_R2);
		r2 = *(float*)&tmpval;

		tmpval = GD5F1GQ4_read_backup_register(LOG_BKP_REG_CALIB_TEMP);
		temperature = *(float*)&tmpval;

		info_byte = (uint8_t)GD5F1GQ4_read_backup_register(LOG_BKP_REG_CALIB_INFO);
		info = *(struct calib_info_struct*)&info_byte;

		timestamp = GD5F1GQ4_read_backup_register(LOG_BKP_REG_CALIB_TIME);

		calib_apply(new_slope, new_intercept, r2, temperature, info);
		calib.timestamp = timestamp;
	}
	else
	{
		struct calib_info_struct calib_info = {0,0,0,0,0};
		calib_apply(NB_LOAD_CELL_DEFAULT_SLOPE, NB_LOAD_CELL_DEFAULT_INTERCEPT,
								1.0, 25.0 , calib_info);

	}
}

// Functions called by CLI:
int calib_get_string(char* buf, int buf_len)
{
	return snprintf(buf, buf_len, "calib, a=%9.6f, b=%lu, r2=%9.6f, temp=%4.1f, id=%u, time=%lu",
			calib.slope, calib.intercept, calib.r2, calib.temperature, calib.info.id, calib.timestamp);
}

int calib_get_info_string(char* buf, int buf_len)
{
	uint16_t samples = 0;
	samples |= calib.info.used_w1 * 0x0001;
	samples |= calib.info.used_w2 * 0x0010;
	samples |= calib.info.used_w3 * 0x0100;
	samples |= calib.info.used_w4 * 0x1000;
	return snprintf(buf, buf_len, "calibinfo, id=%u, time=%lu, samples=%04x", calib.info.id, calib.timestamp, samples);
}

int calib_get_ongoing_info_string(char* buf, int buf_len)
{
	// gather calibration info:
	struct calib_info_struct new_calib_info;
	new_calib_info.id = (calib.info.id + 1) % COMM_N_CALIB_IDS;
	new_calib_info.used_w1 = (calib_events_available[0] > 0);
	new_calib_info.used_w2 = (calib_events_available[1] > 0);
#if(NB_N_CALIBRATION_WEIGHTS > 2)
	new_calib_info.used_w3 = (calib_events_available[2] > 0);
#else
	new_calib_info.used_w3 = 0;
#endif
#if(NB_N_CALIBRATION_WEIGHTS > 3)
	new_calib_info.used_w4 = (calib_events_available[3] > 0);
#else
	new_calib_info.used_w4 = 0;
#endif


	uint16_t samples = 0;
	samples |= new_calib_info.used_w1 * 0x0001;
	samples |= new_calib_info.used_w2 * 0x0010;
	samples |= new_calib_info.used_w3 * 0x0100;
	samples |= new_calib_info.used_w4 * 0x1000;

	uint16_t calib_state = load_cell_get_state();

	return snprintf(buf, buf_len, "calibnew, state=%u, samples=%04x, perch=%u", calib_state, samples, calib_event_state);
}


