/*
 * load_cell.h
 *
 *  Created on: 03 Mar 2018
 *      Author: raffael
 */

#ifndef FW_LOAD_CELL_H_
#define FW_LOAD_CELL_H_

#include "stdint.h"
#include "comm.h"
#include "nestbox.h"
#include "comm_types.h"
#include "calibration.h"

//#define USE_HX
#define USE_ADS

#define LOAD_CELL_N_AVERAGE		10	// number of measurements that get averaged to one sample
#define	LOAD_CELL_N_SAMPLES		30	// maximum number of samples used for the calculation of one detection event

typedef enum loadCellStateEnum{
	LOAD_CELL_UNINIT = 0,
	LOAD_CELL_CALIB,
	LOAD_CELL_INIT,
} load_cell_state_t;

typedef struct loadCellOffset {
	uint32_t timestamp; // only stored in flash
	uint32_t raw_median;
	uint32_t ac_pos_median; // from same sample index like raw_median
	uint32_t ac_neg_median; // from same sample index like raw_median
	int32_t raw_stddev;
	float temperature;

} load_cell_offset_t;

typedef struct loadCellSample {
	int32_t raw_mixed[LOAD_CELL_N_AVERAGE];

	int32_t avg_positive;
	int32_t avg_negative;

	int32_t average;
	float stddev;
} load_cell_sample_t;

typedef enum loadCellResponse{
	LOAD_CELL_BELOW_THRESHOLD,
	LOAD_CELL_ABOVE_THRESHOLD,
	LOAD_CELL_SUFFICIENT_SAMPLES_COLLECTED,
	LOAD_CELL_EVENT_DURATION_ELAPSED,
	LOAD_CELL_ERROR
} load_cell_response_t;

typedef enum loadCellProcessSampleResponse{
	LOAD_CELL_SAMPLE_OK,
	LOAD_CELL_SAMPLE_UNSTABLE,
} load_cell_process_sample_response_t;

typedef enum loadCellCalibResponse{
	LOAD_CELL_CALIBRATION_LOADED_DEFAULT,
	LOAD_CELL_CALIBRATION_LOADED_FROM_MEMORY,
	LOAD_CELL_CALIBRATION_COMPLETED,
} load_cell_calib_response_t;

typedef enum loadCellMeasureOffsetResponse{
	LOAD_CELL_OFFSET_UNCHANGED,
	LOAD_CELL_OFFSET_SIGNIFICANT_CHANGE,
	LOAD_CELL_OFFSET_ERROR_UNSTABLE,
	LOAD_CELL_OFFSET_ERROR_ABORT 		// if aborted due to threshold passed
} load_cell_offset_response_t;

void load_cell_init();
void load_cell_test();

void load_cell_reset_calib_timeout();
load_cell_state_t load_cell_get_state();

load_cell_state_t load_cell_restart_calibration_mode();
load_cell_calib_response_t load_cell_end_calibration_mode();

void load_cell_start_polling_mode();
void load_cell_start_measurement_mode(nestbox_event_t* evt);

void load_cell_acquire_next_sample();

load_cell_response_t load_cell_poll();
load_cell_response_t load_cell_process_sample(nestbox_event_t* evt);
load_cell_process_sample_response_t load_cell_process_event(nestbox_event_t* evt);

load_cell_offset_response_t load_cell_measure_offset();

int load_cell_get_offset(comm_offset_t* of);

int load_cell_get_offset_string(char* buf, int buf_len);
int load_cell_get_weight_string(char* buf, int buf_len);

void load_cell_shutdown();

void load_cell_isr();

#endif /* FW_LOAD_CELL_H_ */
