/*
 * sigfox.h
 *
 *  Created on: 29 Sep 2021
 *      Author: raffael
 */

#ifndef INC_SIGFOX_H_
#define INC_SIGFOX_H_

#include <stdint.h>

#define SIGFOX_DAILY_UPLINK_MESSAGES	70
#define SIGFOX_DAILY_DOWNLINK_MESSAGES	4

#define SIGFOX_DOWNLINK_TIMEOUT		120000	// downlink takes up to one minute.
#define SIGFOX_TX_TIMEOUT			30000	// TX takes ca. 10 seconds

#define SIGFOX_OOB_DELAY_MS			3000

#define SIGFOX_MIN_OFF_TIME_SECONDS		10	// minimum time the module has to be off before turning back on.

#define SIGFOX_DOWNLINK_TIMESTAMP_DELAY_SECONDS		5


#define SIGFOX_FREQ_DEFAULT		868130
#define SIGFOX_FREQ_LOWER		(868130-75)
#define SIGFOX_FREQ_UPPER		(868130+75)

#define SIGFOX_ERROR	1
#define SIGFOX_OK		0

#define SIGFOX_UPLINK_ONLY		0
#define SIGFOX_REQUEST_DOWNLINK	1

#define SIGFOX_DEVICE_ID_LENGTH		8
#define SIGFOX_PAC_LENGTH			16

int sigfox_init(void* delay_ms);

void sigfox_reset_daily_message_counter();

void sigfox_wakeup_module();
void sigfox_module_sleep();
void sigfox_module_deepsleep();


int sigfox_tx_frame(uint8_t* bytes, unsigned int len, int request_downlink);
int sigfox_rx_downlink(uint8_t* bytes, unsigned int timeout_ms);


int sigfox_get_device_id(char* id_buf, int id_buf_len);
int sigfox_get_pac(char* pac_buf, int pac_buf_len);

// EMI test functions:
int sigfox_emi_cw_on(uint32_t freq);
int sigfox_emi_test_pattern_on(uint32_t freq);



#endif /* INC_SIGFOX_H_ */
