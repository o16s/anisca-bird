/*
 * cli.h
 *
 *  Created on: 18 Jan 2022
 *      Author: raffael
 */

#ifndef INC_CLI_H_
#define INC_CLI_H_

#include "main.h"

#define CLI_HOST_RESPONSE_TIMEOUT	10000

typedef struct                        /* describe item in header file */
{
	unsigned char (*cdc_tx)(unsigned char* data, uint16_t len) ;
	int n_tx;
} cli_instance_t;

//Must be called in main initialisation step
void cli_init(cli_instance_t *instance);

//For transitional states, call this function to acknowledge command
void cli_complete_action();

//Must be inserted into CDC_Receive_FS in usb_cdc_if.c file
void cli_cdc_rx(uint8_t * data, uint8_t len);

// Must be inserted into CDC
void cli_TransmitCompleteCallback(uint32_t *Len);


//Processes received string from USB and responds with answer
void cli_process_rx_async();

//Sends the log data through USB
void cli_download_next_log_chunk();

#endif /* INC_CLI_H_ */
