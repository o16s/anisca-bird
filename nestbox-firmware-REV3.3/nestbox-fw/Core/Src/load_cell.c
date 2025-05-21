/*
 * load_cell.c
 *
 *  Created on: 03 Mar 2018
 *      Author: raffael
 */

#include "load_cell.h"
#include "ads1220.h"
#include "rfid.h"
#include "main.h"
#include "rtc.h"

#include <math.h>
#include <string.h>
#include <stdio.h>
#include "usbd_cdc_if.h"
#include "statistics.h"

#define LOAD_CELL_POLARITY_POSITIVE		1
#define LOAD_CELL_POLARITY_NEGATIVE		-1

void ads1220_set_init_loadcell_config(struct Ads1220 *ads, enum Ads1220Mux mux);

static void load_cell_get_weight(int polarity);
static float load_cell_get_temp();

static void load_cell_start_adc(enum Ads1220Mux mux);
static void load_cell_self_test_and_start();
static inline void bridge_supply_off();
static inline void bridge_supply_positive();
static inline void bridge_supply_negative();

static int LoadCellDRDY = 0;
void load_cell_wait_for_ready(unsigned int timeout_ms);
void load_cell_reset_ready();
void load_cell_enable_drdy_int();
void load_cell_disable_drdy_int();

static struct Ads1220 ads;
static load_cell_sample_t sample;
static load_cell_offset_t offset;

static load_cell_state_t load_cell_state;
static int load_cell_connected = 0; // indicates if the load cell has been physically connected.
										// if 0, do not allow negative polarity.
										// (useful at production tests)

static uint8_t temp_flags = 0; // use this variable to store flags that will persist accross events

static unsigned int load_cell_starttime;

void load_cell_init()
{
	load_cell_state = LOAD_CELL_UNINIT;
	load_cell_starttime = rtc_get_timestamp();

	// reset to default values
	offset.raw_median = INT32_MAX;
	offset.ac_pos_median = INT32_MAX;
	offset.ac_neg_median = INT32_MAX;

	// self-test
	load_cell_self_test_and_start();

	// perform an offset measurement immediately:
	load_cell_measure_offset();

	load_cell_state = LOAD_CELL_CALIB;
}

void load_cell_reset_calib_timeout()
{
	load_cell_starttime = rtc_get_timestamp();
}

load_cell_state_t load_cell_get_state()
{
#ifndef NB_CONSTANT_CALIBRATION_MODE
	// This is a timeout that automatically exits the calibration state without processing the samples
	if(load_cell_state == LOAD_CELL_CALIB &&
			rtc_get_timestamp() - load_cell_starttime > NB_AUTO_END_CALIB_STATE)
	{
		HAL_GPIO_WritePin(DEBUG_LED_RTC_CALIB_GPIO_Port, DEBUG_LED_RTC_CALIB_Pin, DEBUG_LED_OFF);
		load_cell_state = LOAD_CELL_INIT;
	}
#endif
	return load_cell_state;
}

load_cell_state_t load_cell_restart_calibration_mode()
{
	if(load_cell_state == LOAD_CELL_UNINIT)
	{
		return LOAD_CELL_UNINIT;

	}
	else
	{
		load_cell_state = LOAD_CELL_CALIB;
		load_cell_reset_calib_timeout();

		return LOAD_CELL_CALIB;
	}
}

load_cell_calib_response_t load_cell_end_calibration_mode()
{
	if(load_cell_state == LOAD_CELL_CALIB)
	{
		if(calib_process_events() == CALIB_OK)
		{
			// mark as initialised:
#ifndef NB_CONSTANT_CALIBRATION_MODE
			load_cell_state = LOAD_CELL_INIT;
#endif
			return LOAD_CELL_CALIBRATION_COMPLETED;
		}
		else
		{
#ifndef NB_CONSTANT_CALIBRATION_MODE
			// if no fresh calibration is performed, the calib loaded at init is used (from flash or default).
			load_cell_state = LOAD_CELL_INIT;
#endif

			return LOAD_CELL_CALIBRATION_LOADED_DEFAULT; // return LOAD_CELL_CALIBRATION_LOADED_FROM_MEMORY if calibration could be loaded from flash
		}
	}
	return LOAD_CELL_CALIBRATION_LOADED_DEFAULT;
}

void load_cell_start_polling_mode()
{
	ads1220_change_mode(&ads, ADS1220_RATE_1000_HZ, ADS1220_SINGLE_SHOT, ADS1220_TEMPERATURE_DISABLED);
	// empirically added: need to trigger a conversion after sample frequency change,
	// as the first data readout is always 0x80,0x00,0x00:
	load_cell_get_weight(LOAD_CELL_POLARITY_POSITIVE);

}

void load_cell_start_measurement_mode(nestbox_event_t* evt)
{
	evt->n_samples = 0;
	evt->flags = 0;

	if(temp_flags & NB_FLAGS_CONTINUOUS_EVENT)
		evt->flags |= NB_FLAGS_CONTINUOUS_EVENT;

	// get temperature:
	evt->temperature = load_cell_get_temp();

	// change mode to 20Hz:
	ads1220_change_mode(&ads, ADS1220_RATE_20_HZ, ADS1220_SINGLE_SHOT, ADS1220_TEMPERATURE_DISABLED);
	// empirically added: need to trigger a conversion after sample frequency change,
	// as the first data readout is always 0x80,0x00,0x00:
	load_cell_get_weight(LOAD_CELL_POLARITY_POSITIVE);

}


load_cell_response_t load_cell_poll()
{
	load_cell_get_weight(LOAD_CELL_POLARITY_POSITIVE);

	if(ads.data > offset.ac_pos_median + calib_get_detection_threshold())
	{
		return LOAD_CELL_ABOVE_THRESHOLD;
	}
	else
	{
		temp_flags &= ~NB_FLAGS_CONTINUOUS_EVENT;
		return LOAD_CELL_BELOW_THRESHOLD;
	}

}

void load_cell_acquire_next_sample()
{
	sample.avg_positive = 0;
	sample.avg_negative = 0;

	// make measurements and sum up in variables holding the average value
	for(int i = 0; i<LOAD_CELL_N_AVERAGE; i++)
	{
		load_cell_get_weight(LOAD_CELL_POLARITY_POSITIVE);
		sample.raw_mixed[i] = ads.data;
		sample.avg_positive += ads.data;

		load_cell_get_weight(LOAD_CELL_POLARITY_NEGATIVE);
		sample.raw_mixed[i] += -ads.data;
		sample.avg_negative += -ads.data;

		sample.raw_mixed[i] = sample.raw_mixed[i]/2;
	}

	sample.average = (sample.avg_positive + sample.avg_negative)/(2*LOAD_CELL_N_AVERAGE);
	sample.avg_positive = sample.avg_positive / LOAD_CELL_N_AVERAGE;
	sample.avg_negative = sample.avg_negative / LOAD_CELL_N_AVERAGE;

	sample.stddev = calculateSD(sample.raw_mixed, LOAD_CELL_N_AVERAGE, sample.average);
}

load_cell_response_t load_cell_process_sample(nestbox_event_t* evt)
{
	load_cell_response_t res = LOAD_CELL_ERROR;

	// did the owl leave the perch?
	if(sample.avg_positive < offset.ac_pos_median + calib_get_detection_threshold())
	{
		res = LOAD_CELL_BELOW_THRESHOLD;
	}
	// is the sample clean enough?
	else if(sample.stddev > calib_get_stdev_max_valid())
	{
		// directly return without storing sample
		res = LOAD_CELL_ABOVE_THRESHOLD;
	}
	else if(evt->n_samples < NB_EVENT_N_SAMPLES)
	{
		if(evt->n_samples > 0)
		{
			// if a previous sample exists, check difference.
			int32_t diff = evt->raw_samples[evt->n_samples - 1] - sample.average;
			if(diff < 0)
				diff = -diff;

			// if difference is too big, delete previous sample:
			if(diff > calib_get_max_sample_diff_valid())
			{
				// this results in previous sample getting overwritten by this sample.
				evt->n_samples = evt->n_samples - 1;
			}
		}

		// add new sample to event
		evt->raw_samples[evt->n_samples] = sample.average;
		evt->ac_pos_samples[evt->n_samples] = sample.avg_positive;
		evt->ac_neg_samples[evt->n_samples] = sample.avg_negative;

		evt->n_samples++;

		if(evt->n_samples < NB_EVENT_N_SAMPLES)
			res = LOAD_CELL_ABOVE_THRESHOLD;
		else
			res = LOAD_CELL_SUFFICIENT_SAMPLES_COLLECTED;
	}

	if(rtc_get_timestamp() - evt->start_timestamp > NB_EVENT_MAX_DURATION)
	{
		res = LOAD_CELL_EVENT_DURATION_ELAPSED;
		temp_flags |= NB_FLAGS_CONTINUOUS_EVENT;
	}

	return res;
}

load_cell_process_sample_response_t load_cell_process_event(nestbox_event_t* evt)
{
	load_cell_process_sample_response_t res = LOAD_CELL_SAMPLE_OK;

	// were any valid samples collected?
	if(evt->n_samples == 0)
	{
		res = LOAD_CELL_SAMPLE_UNSTABLE;
		evt->flags |= NB_FLAGS_INSUFFICIENT_STABILITY;
	}
	else
	{
		// find the median index of all samples (AC-averaged):
		unsigned int med_i = median_index(evt->raw_samples, evt->n_samples, &(evt->raw_median));
		evt->ac_pos_median = evt->ac_pos_samples[med_i];
		evt->ac_neg_median = evt->ac_neg_samples[med_i];

		evt->raw_mean = 0;

		// calculate the mean value:
		for(int i = 0; i<evt->n_samples; i++)
		{
			evt->raw_mean += evt->raw_samples[i];
		}
		evt->raw_mean = evt->raw_mean/evt->n_samples;

		// calculate the standard deviation:
		evt->stddev = calculateSD(evt->raw_samples, evt->n_samples, evt->raw_mean);

		// calculate estimates in grams:
		evt->weight_estimate = (evt->raw_median - offset.raw_median) / calib_get_slope();
		evt->stddev_estimate = evt->stddev / calib_get_slope();
		evt->flags |= (calib_get_id() << NB_FLAGS_CALIB_ID_SHIFT);

		// check if system is still in calibration mode:
		if(load_cell_state == LOAD_CELL_CALIB)
		{
			// store the sample in the calib structure in order to calculate the next calibration factors.
			if(calib_add_event(evt) == CALIB_OK)
				evt->flags |= NB_FLAGS_RECOGNIZED_CALIB_WEIGHT;

		}
	}

	return res;
}

load_cell_offset_response_t load_cell_measure_offset()
{
	nestbox_event_t offset_evt;

	load_cell_start_measurement_mode(&offset_evt);

	unsigned int timeout = 0;

	while(offset_evt.n_samples < NB_OFFSET_N_SAMPLES)
	{
		load_cell_acquire_next_sample();

		// is weight above threshold again? --> abort!
		if(sample.avg_positive > offset.ac_pos_median + calib_get_detection_threshold())
		{
			load_cell_start_polling_mode();
			return LOAD_CELL_OFFSET_ERROR_ABORT;
		}
		// is the sample clean enough?
		else if(sample.stddev > calib_get_stdev_max_valid())
		{
			if(timeout > NB_OFFSET_UNSTABLE_RETRY_TIMEOUT)
				return LOAD_CELL_OFFSET_ERROR_UNSTABLE;

			// increment timeout to avoid getting blocked here.
			timeout++;
			// acquire next sample
			continue;
		}
		// else:

		// add new sample to event
		offset_evt.raw_samples[offset_evt.n_samples] = sample.average;
		offset_evt.ac_pos_samples[offset_evt.n_samples] = sample.avg_positive;
		offset_evt.ac_neg_samples[offset_evt.n_samples] = sample.avg_negative;

		offset_evt.n_samples++;

	}

	load_cell_offset_response_t res = LOAD_CELL_OFFSET_UNCHANGED;

	// compute new offset value from sample:
	if(load_cell_process_event(&offset_evt) == LOAD_CELL_SAMPLE_OK)
	{
		// check if there was significant change:
		if(abs(offset.raw_median - offset_evt.raw_median) > NB_OFFSET_SIGNIFICANT_CHANGE * calib_get_slope())
			res = LOAD_CELL_OFFSET_SIGNIFICANT_CHANGE;

		offset.ac_neg_median = offset_evt.ac_neg_median;
		offset.ac_pos_median = offset_evt.ac_pos_median;
		offset.raw_median = offset_evt.raw_median;
		offset.raw_stddev = offset_evt.stddev;
		offset.temperature = offset_evt.temperature;
		offset.timestamp = rtc_get_timestamp();
	}

	load_cell_start_polling_mode();

	return res;
}

int32_t pos_weight;
int32_t neg_weight;

// void load_cell_test()
// {
// 	load_cell_init(NULL);

// 	char outbuf[30];
// 	int strlen = 0;

// 	while(1)
// 	{
// 		load_cell_get_weight(LOAD_CELL_POLARITY_POSITIVE);
// 		pos_weight = ads.data;

// 		load_cell_get_weight(LOAD_CELL_POLARITY_NEGATIVE);
// 		neg_weight = ads.data;

// 		memset(outbuf, 0, 30);
// 		strlen=sprintf(outbuf, "%ld, %ld\n", pos_weight, neg_weight);

// 		CDC_Transmit_FS((uint8_t*)outbuf, strlen);
// 		HAL_GPIO_TogglePin(DEBUG_LED_RTC_CALIB_GPIO_Port, DEBUG_LED_RTC_CALIB_Pin);
// 	}

// }

int load_cell_get_offset(comm_offset_t* of)
{
	of->flags = // todo-ok
	of->weight = round(offset.raw_median * 100.0 / calib_get_slope());
	of->raw_adc_pos = offset.ac_pos_median >> NB_RAW_VALUE_BIT_SHIFT; // shifted by 5 bits
	of->raw_adc_neg = offset.ac_neg_median >> NB_RAW_VALUE_BIT_SHIFT;
	of->stddev = round(offset.raw_stddev * 10.0 / calib_get_slope());
	of->temperature = round(offset.temperature);

	return 0;
}

// Functions called by CLI:
int load_cell_get_offset_string(char* buf, int buf_len)
{
	return snprintf(buf, buf_len, "offset,%lu, stddev=%li, temp=%4.1f", offset.raw_median, offset.raw_stddev, offset.temperature);
}

int load_cell_get_weight_string(char* buf, int buf_len)
{
	nestbox_event_t dummy_evt;

	load_cell_start_measurement_mode(&dummy_evt);
	load_cell_acquire_next_sample();
	load_cell_start_polling_mode();

	return snprintf(buf, buf_len, "weight,%lu", sample.average);
}

//////////////////////// helper functions ///////////////////////////

void ads1220_set_init_loadcell_config(struct Ads1220 *ads, enum Ads1220Mux mux){
	ads->config.mux = mux;
	ads->config.gain = ADS1220_GAIN_128;
	ads->config.pga_bypass = 0;
	ads->config.rate = ADS1220_RATE_20_HZ;
	// todo-ok: change operating mode to duty-cycle mode
	ads->config.conv = ADS1220_SINGLE_SHOT;
	ads->config.temp_sensor = ADS1220_TEMPERATURE_DISABLED;
//	ads->config.vref = ADS1220_VREF_EXTERNAL_AIN; // this will be toggled with AC excitation
	ads->config.vref = ADS1220_VREF_EXTERNAL_REF; // this will be toggled with AC excitation
	ads->config.idac = ADS1220_IDAC_OFF;
	ads->config.i1mux = ADS1220_IMUX_OFF;
	ads->config.i2mux = ADS1220_IMUX_OFF;
	ads->config.low_switch = 0; // If 1, internal switch is closed after START/SYNC command
	ads->config.filter = ADS1220_FILTER_NONE; //At data rates of 5 SPS and 20 SPS, the filter can be configured to reject 50-Hz or 60-Hz line frequencies or to simultaneously reject 50 Hz and 60 Hz!!!
}


static void load_cell_get_weight(int polarity)
{
	// TODO-NEXT: first call of this function returns zero to ads.data --> check why!

	// turn off all supplies
	bridge_supply_off();

	HAL_Delay(1); // after supply off.

	if(polarity == LOAD_CELL_POLARITY_POSITIVE || load_cell_connected == 0)
	{
		bridge_supply_positive();
		ads1120_change_polarity(&ads, ADS1220_VREF_EXTERNAL_REF);
	}
	else if(polarity == LOAD_CELL_POLARITY_NEGATIVE)
	{
		bridge_supply_negative();
		ads1120_change_polarity(&ads, ADS1220_VREF_EXTERNAL_AIN);

	}

	HAL_Delay(1); // after polarity change.

	load_cell_reset_ready();
	load_cell_enable_drdy_int();

	ads1220_start_conversion(&ads);
	load_cell_wait_for_ready(100);

	ads1220_periodic(&ads);
	ads1220_event(&ads);

	// turn off all supplies
	bridge_supply_off();
//	load_cell_disable_drdy_int();
}


static float load_cell_get_temp()
{
	// measure temperature
	ads1220_change_mode(&ads, ADS1220_RATE_20_HZ, ADS1220_SINGLE_SHOT, ADS1220_TEMPERATURE_ENABLED); // used to be continuous mode (!)
	load_cell_enable_drdy_int();

	load_cell_reset_ready();
	ads1220_start_conversion(&ads);
	load_cell_wait_for_ready(100); // timeout 100 ms in case DRDY pin is not connected

	ads1220_read(&ads);
	ads1220_event(&ads);

	float temp = ads1220_convert_temperature(&ads);

	load_cell_disable_drdy_int();

	return temp;
}


static inline void bridge_supply_off()
{
	HAL_GPIO_WritePin(EXC_B_P_GPIO_Port, EXC_B_P_Pin, 1);
	HAL_GPIO_WritePin(EXC_A_N_GPIO_Port, EXC_A_N_Pin, 0);
	HAL_GPIO_WritePin(EXC_A_P_GPIO_Port, EXC_A_P_Pin, 1);
	HAL_GPIO_WritePin(EXC_B_N_GPIO_Port, EXC_B_N_Pin, 0);
}

static inline void bridge_supply_negative()
{
	// turn on supply A: +  and B: -
	HAL_GPIO_WritePin(EXC_A_P_GPIO_Port, EXC_A_P_Pin, 0);
	HAL_GPIO_WritePin(EXC_B_N_GPIO_Port, EXC_B_N_Pin, 1);
}
static inline void bridge_supply_positive()
{
	// turn on supply A: -  and B: +
	HAL_GPIO_WritePin(EXC_B_P_GPIO_Port, EXC_B_P_Pin, 0);
	HAL_GPIO_WritePin(EXC_A_N_GPIO_Port, EXC_A_N_Pin, 1);
}

static void load_cell_self_test_and_start()
{
	load_cell_start_adc(ADS1220_MUX_AIN1_AVSS);

	load_cell_get_weight(LOAD_CELL_POLARITY_POSITIVE);

	if(ads.data >= ADS_MAX_ADC_VAL)
		load_cell_connected = 1;
	else
		load_cell_connected = 0;

	load_cell_start_adc(ADS1220_MUX_AIN1_AIN2);
}

static void load_cell_start_adc(enum Ads1220Mux mux)
{
	// Important: turn off all load cell supply FETs
	bridge_supply_off();

	HAL_Delay(100); //wait until things are settled...

	/* Initialize the Data READY semaphore */

	// turn on analog supply:
	HAL_GPIO_WritePin(LOAD_CELL_LDO_EN_GPIO_Port, LOAD_CELL_LDO_EN_Pin, 1);

	HAL_Delay(10);

	ads1220_init(&ads);
	ads1220_set_init_loadcell_config(&ads, mux);
	HAL_Delay(10);
	ads1220_event(&ads);
	ads1220_configure(&ads); // this one sends the reset and config
}

void load_cell_shutdown()
{
	bridge_supply_off();
    ads1220_powerdown(&ads); //very important!

}

inline void load_cell_reset_ready()
{
	__disable_irq();
	LoadCellDRDY = 0;
	__enable_irq();
}

inline void load_cell_wait_for_ready(unsigned int timeout_ms)
{
	unsigned int starttimestamp = HAL_GetTick();
	while(HAL_GetTick()-starttimestamp < timeout_ms)
	{
		if(LoadCellDRDY)
			break;
	}
	load_cell_reset_ready();
}

void load_cell_disable_drdy_int()
{
	HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
}

void load_cell_enable_drdy_int()
{
	HAL_NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

inline void load_cell_isr()
{
	bridge_supply_off();
	LoadCellDRDY = 1;
}
