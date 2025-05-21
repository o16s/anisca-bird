/*
 * nestbox.h
 *
 *  Created on: 15 Sep 2021
 *      Author: raffael
 */

#ifndef INC_NESTBOX_H_
#define INC_NESTBOX_H_

#include "oi_gitcommit.h"

/******************** Main configurations: **************************/
//#define DEBUG_WITHOUT_COMM_MODULE
//#define NB_CONSTANT_CALIBRATION_MODE
#define USE_LOW_POWER_MODE
#define NB_DAYTIME_POWER_OFF 	//comment if system shall be on 24/24h
//#define NB2_EMC_TEST_MODE		// uncomment for emission testing
/********************************************************************/

// Version info
#define NB_HW_VERSION					0x33
#define NB_FW_VERSION					OI_GIT_HEAD

// Load cell defaults:
#define NB_DETECTION_THRESHOLD			50		// grams
#define NB_OVERLOAD_THRESHOLD			2000	// grams
#define NB_MAX_STDEV_FOR_VALID_SAMPLE	5	// grams
#define NB_MAX_DIFF_FOR_VALID_SAMPLE	5	// maximum grams of difference between two samples to be considered valid.

#define NB_OFFSET_SIGNIFICANT_CHANGE	2	// grams

#define NB_LOAD_CELL_DEFAULT_SLOPE		500	// ADC counts per gram
#define NB_LOAD_CELL_DEFAULT_INTERCEPT	0	// ADC counts

// Calibration defaults:
#define NB_N_CALIBRATION_WEIGHTS			4
#define NB_CAL_W0							100.0
#define NB_CAL_W1							200.0
#define NB_CAL_W2							500.0
#define NB_CAL_W3							1000.0
#define NB_CALIBRATION_WEIGHTS_TOL			0.25 	// fraction; load cell has a sensitivity tolerance of 10%; then temperature effects need to be taken into account as well.

#define NB_DEFAULT_CALIBRATION_WEIGHTS 		{NB_CAL_W0,NB_CAL_W1,NB_CAL_W2,NB_CAL_W3} // in grams, floating point
#define NB_CALIBRATION_WEIGHTS_TOLERANCES	{NB_CAL_W0 * NB_CALIBRATION_WEIGHTS_TOL, NB_CAL_W1 * NB_CALIBRATION_WEIGHTS_TOL, \
											NB_CAL_W2 * NB_CALIBRATION_WEIGHTS_TOL, NB_CAL_W3 * NB_CALIBRATION_WEIGHTS_TOL} // in grams, floating point


#define NB_N_MINIMUM_CALIBRATION_WEIGHTS	3	// minimum number of weights to be used for calibration
#define NB_LINREG_MIN_R2					0.999f


// Power Management:
#define VBAT_FULL_MV		5400 // value used to calculate battery percentage (empirical from field study)
#define VBAT_EMPTY_MV		4500 // value used to calculate battery percentage (empirical from field study)
#define VBAT_SHUTDOWN_MV	4500 // system sends a last comm message before going to deep sleep
#define VBAT_STARTUP_MIN_MV	4600 // system will not boot from POR if battery voltage is below this value.

#define VBAT_SHUTDOWN_THRESHOLD_COUNTER		6


// Daytime power-off management:
#define NB_DAYTIME_POWER_OFF_HOUR_UTC	8	// system will switch off at 8am UTC (9am CET, 10am CEST)
#define NB_DAYTIME_POWER_ON_HOUR_UTC	16	// system will switch on at 4pm UTC (5pm CET, 6pm CEST)


// Timeout values in number of retries
#define NB_RFID_RETRY_TIMEOUT				10
#define NB_OFFSET_UNSTABLE_RETRY_TIMEOUT	10
#define NB_GET_TIME_RETRY_TIMEOUT			3

// Time values in milliseconds
#define RFID_READER_ON_TIME_MS		500
// Time values in seconds:
#define NB_RFID_RETRY_PERIOD		3
#define NB_EVENT_MAX_DURATION		240
// intervals in seconds:
#define NB_LOAD_CELL_POLLING_PERIOD	1
#define	NB_OFFSET_MEAS_PERIOD		180
#define NB_OFFSET_MAX_TX_PERIOD		7200//14400	// transmit offset at least every 4 hours
#define NB_OFFSET_MIN_TX_PERIOD		3600	// transmit offset at most every 1 hour

#define	NB_STATUS_MEAS_PERIOD		180
#define	NB_STATUS_TX_PERIOD			28800	// transmit status every 8 hours
#define NB_GET_TIME_RETRY_PERIOD	1800	// if base station did not reply, retry fetching the time.
#define NB_AUTO_END_CALIB_STATE		3600	// after 1 hour, the system automatically exits calibration mode

#define NB_EVENT_N_SAMPLES			19	// preferably an uneven number (for median calculation)
#define NB_OFFSET_N_SAMPLES			11	// must be smaller than NB_EVENT_N_SAMPLES!
											// + preferably an uneven number (for median calculation)
#if(NB_OFFSET_N_SAMPLES > NB_EVENT_N_SAMPLES)
#error("NB_EVENT_N_SAMPLES must be larger than or equal NB_OFFSET_N_SAMPLES")
#endif


#define NB_RAW_VALUE_BIT_SHIFT		5	// raw ADC values are shifted by 5 (divided by 32) when getting sent via sigfox.

typedef struct nestboxEvent {
	uint32_t start_timestamp;
	uint32_t stop_timestamp;

	uint64_t rfid_uid;

	unsigned int n_samples;

	int32_t raw_samples[NB_EVENT_N_SAMPLES];		// mean value from AC_pos and AC_neg
	int32_t ac_pos_samples[NB_EVENT_N_SAMPLES];
	int32_t ac_neg_samples[NB_EVENT_N_SAMPLES];

	int32_t stddev;
	int32_t raw_mean;
	int32_t raw_median;
	int32_t ac_pos_median; // from same sample index like raw_median
	int32_t ac_neg_median; // from same sample index like raw_median

	float weight_estimate;	// in grams
	float stddev_estimate;	// in grams

	uint8_t flags;
	float temperature;
} nestbox_event_t;

// event functions:
void nestbox_request_cli_process();
void nestbox_request_usb_state_change();
void nestbox_request_complete_startup();

void nestbox_main();

#endif /* INC_NESTBOX_H_ */
