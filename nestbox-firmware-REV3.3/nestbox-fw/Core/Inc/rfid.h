/*
 * rfid.h
 *
 *  Created on: 29 Jun 2021
 *      Author: raffael
 */

#ifndef INC_RFID_H_
#define INC_RFID_H_

#include <stdint.h>
#include "nestbox.h"

#define RFID_TIMEOUT_OR_FAILED 	1
#define RFID_SUCCESS			0

void rfid_test();

void rfid_on();
void rfid_off();

int rfid_detect_id();

int rfid_get_compressed_id(nestbox_event_t* evt);

void lf_tag_read_isr();

int rfid_get_string(char* buf, int buf_len);

#endif /* INC_RFID_H_ */
