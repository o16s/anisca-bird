/*
 * calibration.h
 *
 *  Created on: 1 Oct 2021
 *      Author: raffael.tschui
 */

#ifndef INC_CALIBRATION_H_
#define INC_CALIBRATION_H_

#include "comm.h"
#include "nestbox.h"

#define CALIB_OK		0
#define CALIB_ERROR		1

#define CALIB_NO_WEIGHT_ON_PERCH			0
#define CALIB_WEIGHT_DETECTED				1
#define CALIB_WEIGHT_MEASUREMENT_ONGOING	2
#define CALIB_WEIGHT_MEASUREMENT_COMPLETED	3


typedef struct loadCellCalib {
	uint32_t timestamp; // only stored in flash

	float r2; 		// linear regression R^2
	int32_t intercept;	// raw ADC value
	float slope; 	// divide raw ADC by slope to obtain weight
	float temperature;

	// note: the below values are added to last measured offset value and therefore the detection thresholds vary over time
	int32_t detection_threshold; // raw ADC counts added to offset which result in event start
	int32_t overload_threshold;	// "-----" result in a fault event
	int32_t stdev_max_valid;	// raw ADC counts of a valid standard deviation
	int32_t sample_diff_max_valid; // raw ADC counts of the maximum permissible difference between two samples in an event.

	struct calib_info_struct info;

} load_cell_calib_t;


float calib_get_slope();
int32_t calib_get_detection_threshold();
int32_t calib_get_stdev_max_valid();
int32_t calib_get_max_sample_diff_valid();
uint8_t calib_get_id();

void calib_reset_ongoing(); // deletes all current calibration weights (for CLI)
int calib_new_ongoing();

#define CALIB_BACKUP_PW		0x12ca1b55
void calib_backup();
void calib_restore();

int calib_add_event(nestbox_event_t* evt);
int calib_process_events();

// update state being used in CLI command (also update if not in calibration mode):
void calib_inform_weight_detected();
void calib_inform_weight_measurement_ongoing();
void calib_inform_weight_measurement_completed();
void calib_inform_weight_removed();


void linregtest();

int calib_pack_message(comm_calib_t* clb);
int calib_get_string(char* buf, int buf_len);
int calib_get_info_string(char* buf, int buf_len);
int calib_get_ongoing_info_string(char* buf, int buf_len);

#endif /* INC_CALIBRATION_H_ */
