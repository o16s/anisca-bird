/*
 * comm.h
 *
 *  Created on: 15 Sep 2021
 *      Author: raffael
 */

#ifndef INC_COMM_H_
#define INC_COMM_H_

#include <stdint.h>
#include "nestbox.h"
#include "comm_types.h"

#define COMM_OK			0
#define COMM_ERROR		1

#define COMM_UPLINK_ONLY	0
#define COMM_REQUEST_ANSWER	1

#define LOG_TO_MEM_ONLY			1
#define LOG_TO_MEM_AND_TX		0


void comm_init();
void comm_wakeup_get_time_from_basestation();

void comm_module_off();



// TX messages:
int comm_send_status(int request_downlink, int log_only);
void comm_send_offset(int log_only);
void comm_send_calibration();
void comm_send_event(nestbox_event_t* evt, int log_only);

// CLI functions:
int comm_get_info_string(char* buf, int buf_len);
int comm_get_downlink_string(char* buf, int buf_len);
void comm_force_tx_once();

// EMC test functions:
void comm_emi_test_cw_on(uint32_t freq);
void comm_emi_test_pattern_on(uint32_t freq);
void comm_emi_test_off();


#endif /* INC_COMM_H_ */
