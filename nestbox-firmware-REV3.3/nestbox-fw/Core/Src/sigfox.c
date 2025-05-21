/*
 * sigfox.c
 *
 *  Created on: 29 Sep 2021
 *      Author: raffael
 */

#include "sigfox.h"
#include "usart.h"
#include <stdint.h>
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include "log.h"

#define SIGFOX_TX_BUF_LEN				64
#define SIGFOX_RX_BUF_LEN				64

#define SIGFOX_UART_TIMEOUT			100

#define SIGFOX_MAX_FRAME_LENGTH		12	// limited by sigfox
#define SIGFOX_DOWNLINK_N_BYTES		8

#define SIGFOX_RX_NEWLINE_LENGTH	2

const static char* at_test = "AT\r\n";
const static char at_send_frame[6] = "AT$SF=";
const static char* at_downlink_rx = "RX=";
const static char* at_sleep = "AT$P=1\r\n";
const static char* at_deepsleep = "AT$P=2\r\n";
const static char* at_emi_cw = "AT$CW=868130000,1\r\n";
const static char* at_emi_cw_upper = "AT$CW=868205000,1\r\n";
const static char* at_emi_cw_lower = "AT$CW=868055000,1\r\n";

const static char* at_emi_set_freq_default = "AT$IF=868130000\r\n";
const static char* at_emi_set_freq_upper = "AT$IF=868205000\r\n";
const static char* at_emi_set_freq_lower = "AT$IF=868055000\r\n";
const static char* at_emi_test_pattern = "AT$CB=-1,1\r\n";
const static char* at_get_device_id = "AT$I=10\r\n";
const static char* at_get_pac = "AT$I=11\r\n";


static void (*sigfox_delay_ms)(uint32_t);
char sigfox_tx_buffer[SIGFOX_TX_BUF_LEN];
char sigfox_rx_buffer[SIGFOX_RX_BUF_LEN];

static int sigfox_daily_uplink_counter = 0;
static int sigfox_daily_downlink_counter = 0;

static int sigfox_rx_sequence(const char* sequence, int len, unsigned int timeout_ms);

void sigfox_wakeup_module()
{
	MX_USART3_UART_Init();
	// TODO-OK: GPIO 9 of module wakes up from deepsleep.
}
void sigfox_module_sleep()
{
	io_uart_tx((uint8_t*)at_sleep, strlen(at_sleep));
	uart_DeInit();

}

void sigfox_module_deepsleep()
{
	io_uart_tx((uint8_t*)at_deepsleep, strlen(at_deepsleep));
	uart_DeInit();

}

void sigfox_reset_daily_message_counter()
{
	sigfox_daily_uplink_counter = 0;
	sigfox_daily_downlink_counter = 0;
}


int sigfox_tx_frame(uint8_t* bytes, unsigned int len, int request_downlink)
{
	if(len > SIGFOX_MAX_FRAME_LENGTH || sigfox_daily_uplink_counter >= SIGFOX_DAILY_UPLINK_MESSAGES)
		return SIGFOX_ERROR;

	if(request_downlink && sigfox_daily_downlink_counter >= SIGFOX_DAILY_UPLINK_MESSAGES)

	memset(sigfox_tx_buffer, 0, SIGFOX_TX_BUF_LEN);

	memcpy(sigfox_tx_buffer, at_send_frame, 6);
	char* tmp = &sigfox_tx_buffer[6];

	for(int i = 0; i < len; i++)
	{
		sprintf(tmp, "%02X", bytes[i]);
		tmp+=2;
	}

	if(request_downlink)
		sprintf(tmp, ",1\r\n");
	else
		sprintf(tmp, "\r\n");


	if(io_uart_tx((uint8_t*)sigfox_tx_buffer, strnlen(sigfox_tx_buffer,SIGFOX_TX_BUF_LEN)) == HAL_OK)
	{
		// if requested downlink, do not wait for OK sequence, but directly return.
		if(request_downlink)
		{
			sigfox_daily_uplink_counter++;
			return SIGFOX_OK;
		}

		// wait for receiving response: OK
		if(sigfox_rx_sequence("OK", 2, SIGFOX_TX_TIMEOUT))
		{
			sigfox_daily_uplink_counter++;
			return SIGFOX_OK;
		}
	}
	return SIGFOX_ERROR;

}

int sigfox_rx_downlink(uint8_t* bytes, unsigned int timeout_ms)
{
	if(sigfox_daily_downlink_counter >= SIGFOX_DAILY_DOWNLINK_MESSAGES)
		return SIGFOX_ERROR;

	int index = sigfox_rx_sequence(at_downlink_rx, strlen(at_downlink_rx), timeout_ms);

	if(index)
	{
		// extract hexstring from downlink message:
		char* endptr = &sigfox_rx_buffer[index];
		for(int i = 0; i<SIGFOX_DOWNLINK_N_BYTES; i++)
		{
			bytes[i] = (uint8_t)strtol(endptr, &endptr, 16);
			endptr++; // space character

			if(endptr < &sigfox_rx_buffer[SIGFOX_RX_BUF_LEN])
				continue;
			else
				return SIGFOX_ERROR;
		}

		sigfox_daily_downlink_counter++;
		return SIGFOX_OK;
	}

	return SIGFOX_ERROR;
}


int sigfox_init(void* delay_ms)
{
	sigfox_delay_ms = delay_ms;
	memset(sigfox_tx_buffer, 0, SIGFOX_TX_BUF_LEN);

	// comm test:
	io_uart_tx((uint8_t*)at_test, strlen(at_test));

	if(sigfox_rx_sequence("OK", 2, SIGFOX_UART_TIMEOUT))
		return SIGFOX_OK;

	return SIGFOX_ERROR;
}

// TODO-OK change to interrupt mode and go to sleep.
static int sigfox_rx_ln(unsigned int timeout_ms, int min_length)
{
	unsigned int start = HAL_GetTick();

	uint8_t* rx_pointer;

	while(HAL_GetTick() - start < timeout_ms)
	{
//		io_uart_rx((uint8_t*)rx_pointer, 1, timeout_ms-(HAL_GetTick() - start));
//
//		if(*rx_pointer == SIGFOX_NEWLINE_CHARACTER)
//			return rx_pointer - sigfox_rx_buffer+1;
//		else
//			rx_pointer++;

		int linelength = io_uart_rx_line(&rx_pointer, timeout_ms - (HAL_GetTick() - start));

		if(linelength >= min_length)
		{
			memcpy(sigfox_rx_buffer, rx_pointer, linelength);
			return linelength;
		}

	}
	return 0;
}

/*
 * returns the first index after the desired byte sequence
 */
static int sigfox_rx_sequence(const char* sequence, int seq_len, unsigned int timeout_ms)
{
	unsigned int start = HAL_GetTick();

	while(HAL_GetTick() - start < timeout_ms)
	{
		int nchar = sigfox_rx_ln(timeout_ms - (HAL_GetTick() - start), seq_len);

		for(int i=0; i<=nchar-seq_len; i++)
		{
			if(strncmp(&sigfox_rx_buffer[i], sequence, seq_len) == 0)
			{
				return i + seq_len;
			}
		}
	}
	return 0;
}

int sigfox_get_device_id(char* id_buf, int id_buf_len)
{
	if(io_uart_tx((uint8_t*)at_get_device_id, strnlen(at_get_device_id,SIGFOX_TX_BUF_LEN)) == HAL_OK)
	{
		// response = module ID
		int nchar = sigfox_rx_ln(SIGFOX_UART_TIMEOUT, SIGFOX_DEVICE_ID_LENGTH+SIGFOX_RX_NEWLINE_LENGTH);

		if(nchar == SIGFOX_DEVICE_ID_LENGTH+SIGFOX_RX_NEWLINE_LENGTH
				&& id_buf_len >= SIGFOX_DEVICE_ID_LENGTH)
		{
			memcpy(id_buf, sigfox_rx_buffer, SIGFOX_DEVICE_ID_LENGTH);
			return SIGFOX_DEVICE_ID_LENGTH;
		}
	}
	return 0;
}

int sigfox_get_pac(char* pac_buf, int pac_buf_len)
{
	if(io_uart_tx((uint8_t*)at_get_pac, strnlen(at_get_pac,SIGFOX_TX_BUF_LEN)) == HAL_OK)
	{
		// response = PAC
		int nchar = sigfox_rx_ln(SIGFOX_UART_TIMEOUT, SIGFOX_PAC_LENGTH+SIGFOX_RX_NEWLINE_LENGTH);

		if(nchar == SIGFOX_PAC_LENGTH+SIGFOX_RX_NEWLINE_LENGTH
				&& pac_buf_len >= SIGFOX_PAC_LENGTH)
		{
			memcpy(pac_buf, sigfox_rx_buffer, SIGFOX_PAC_LENGTH);
			return SIGFOX_PAC_LENGTH;
		}
	}
	return 0;
}

int sigfox_emi_cw_on(uint32_t freq)
{
	switch(freq)
	{
	case SIGFOX_FREQ_UPPER:
		if(io_uart_tx((uint8_t*)at_emi_cw_upper, strnlen(at_emi_cw_upper,SIGFOX_TX_BUF_LEN)) != HAL_OK)
			return SIGFOX_ERROR;
		break;
	case SIGFOX_FREQ_LOWER:
		if(io_uart_tx((uint8_t*)at_emi_cw_lower, strnlen(at_emi_cw_lower,SIGFOX_TX_BUF_LEN)) != HAL_OK)
			return SIGFOX_ERROR;
		break;
	case SIGFOX_FREQ_DEFAULT:
		if(io_uart_tx((uint8_t*)at_emi_cw, strnlen(at_emi_cw,SIGFOX_TX_BUF_LEN)) != HAL_OK)
			return SIGFOX_ERROR;
		break;
	default:
		return SIGFOX_ERROR;
	}

//		// wait for receiving response: OK
//		if(sigfox_rx_sequence("OK", 2, SIGFOX_TX_TIMEOUT))
//			return SIGFOX_OK;

	return SIGFOX_OK;

}

int sigfox_emi_test_pattern_on(uint32_t freq)
{
	// change frequency to requested value:
	switch(freq)
	{
	case SIGFOX_FREQ_UPPER:
		if(io_uart_tx((uint8_t*)at_emi_set_freq_upper, strnlen(at_emi_set_freq_upper,SIGFOX_TX_BUF_LEN)) != HAL_OK)
			return SIGFOX_ERROR;
		break;
	case SIGFOX_FREQ_LOWER:
		if(io_uart_tx((uint8_t*)at_emi_set_freq_lower, strnlen(at_emi_set_freq_lower,SIGFOX_TX_BUF_LEN)) != HAL_OK)
			return SIGFOX_ERROR;
		break;
	case SIGFOX_FREQ_DEFAULT:
		if(io_uart_tx((uint8_t*)at_emi_set_freq_default, strnlen(at_emi_set_freq_default,SIGFOX_TX_BUF_LEN)) != HAL_OK)
			return SIGFOX_ERROR;
		break;
	default:
		return SIGFOX_ERROR;
	}

	// wait for receiving response: OK (not sure if this is what we expect, therefore no test was written
	sigfox_rx_sequence("OK", 2, SIGFOX_UART_TIMEOUT);


	// start the modulated TX:
	if(io_uart_tx((uint8_t*)at_emi_test_pattern, strnlen(at_emi_test_pattern,SIGFOX_TX_BUF_LEN)) != HAL_OK)
	{
		return SIGFOX_ERROR;

//		// wait for receiving response: OK
//		if(sigfox_rx_sequence("OK", 2, SIGFOX_TX_TIMEOUT))
//			return SIGFOX_OK;
	}
	return SIGFOX_OK;

}
