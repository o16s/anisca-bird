/*
 * cli.c
 *
 *  Created on: 18 Jan 2022
 *      Author: raffael
 */

#include "cli.h"
#include "rtc.h"
#include "boot.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "usb_device.h"
#include "system.h"
#include "log.h"
#include "adc.h"
#include "rfid.h"
#include "load_cell.h"
#include "sigfox.h"

#define INFO_LEN				 200
#define MAX_LEN_RECEIVED_LINE 	 64
#define LOG_ENTRIES_PER_CHUNK	1800
#define LOG_ENTRIES_PER_GROUP	24

static cli_instance_t * _instance;
static char _received[MAX_LEN_RECEIVED_LINE];
static uint16_t _received_len = 0;

unsigned int total_printed_log_entry_iterator = 0;

#ifdef CLI_ENCODE_B64
const char b64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

size_t b64_encoded_size(size_t inlen)
{
	size_t ret;

	ret = inlen;
	if (inlen % 3 != 0)
		ret += 3 - (inlen % 3);
	ret /= 3;
	ret *= 4;

	return ret;
}

size_t b64_encode(char* in, size_t len, char* out)
{
	size_t  elen;
	size_t  i;
	size_t  j;
	size_t  v;

	if (in == NULL || len == 0)
		return 0;

	elen = b64_encoded_size(len);
	out[elen] = '\0';

	for (i=0, j=0; i<len; i+=3, j+=4) {
		v = in[i];
		v = i+1 < len ? v << 8 | in[i+1] : v << 8;
		v = i+2 < len ? v << 8 | in[i+2] : v << 8;

		out[j]   = b64chars[(v >> 18) & 0x3F];
		out[j+1] = b64chars[(v >> 12) & 0x3F];
		if (i+1 < len) {
			out[j+2] = b64chars[(v >> 6) & 0x3F];
		} else {
			out[j+2] = '=';
		}
		if (i+2 < len) {
			out[j+3] = b64chars[v & 0x3F];
		} else {
			out[j+3] = '=';
		}
	}

	return elen;
}
#endif

void clearReceived(){
	memset(_received, 0, MAX_LEN_RECEIVED_LINE);
}

void cli_TransmitCompleteCallback(uint32_t *Len)
{
	_instance->n_tx -= *Len;
}

USBD_StatusTypeDef usbSafeTransmit(char* data, uint16_t len)
{
	int cli_watchdog_counter = 0;

	while(_instance->cdc_tx((unsigned char*)data, len) != USBD_OK)
	{
		// check if usb is still connected:
		if(system_get_usb_connection_state() == USB_DISCONNECTED)
			return USBD_FAIL;

		// exit USB communication if host no longer responds after a timeout
		if(cli_watchdog_counter > CLI_HOST_RESPONSE_TIMEOUT)
			return USBD_FAIL;

		HAL_Delay(1);
		cli_watchdog_counter++;
	}

	_instance->n_tx += len;

	cli_watchdog_counter = 0;
	while(_instance->n_tx)
	{
		if(cli_watchdog_counter > CLI_HOST_RESPONSE_TIMEOUT)
		{
			_instance->n_tx = 0;
			return USBD_FAIL;
		}

		HAL_Delay(1);
		cli_watchdog_counter++;
	}
	return USBD_OK;
}

USBD_StatusTypeDef respondLine(char * str, uint8_t len){
	char linefeed = '\n';
	usbSafeTransmit(str, (uint8_t)len);
	return usbSafeTransmit(&linefeed, 1);
}

void cli_init(cli_instance_t *instance){
	_instance = instance;
	clearReceived();
}

void cli_complete_action()
{
	respondLine("ok", strlen("ok"));
}


void cli_process_rx_IT()
{
	if(strstr(_received, "\n") == NULL) return;


	else
	{
		// could not be processed immediately --> trigger asynchronous USB event
		nestbox_request_cli_process();
	}

}

void cli_process_rx_async(){

	if(strstr(_received, "\n") == NULL) return;

	char info_response[INFO_LEN];
	memset(info_response, 0, INFO_LEN);

	if (_received[0] == 'i') { //get device state

		unsigned int timestamp = rtc_get_timestamp();

		uint8_t battery_percent = io_battery_level();
		int battery_voltage = io_get_last_battery_voltage();

		unsigned int log_entries = log_get_counter();
		uint8_t hw_version = NB_HW_VERSION;
		uint64_t serial0 = HAL_GetUIDw0() + HAL_GetUIDw2();
		uint64_t serial1 = HAL_GetUIDw1();

		serial0 &= 0x00000000ffffffff;
		serial1 = serial1 >> 16;

		snprintf(info_response, INFO_LEN,
					"nestbox,%u,%u,%08X%04X,%X,%d,%3.4f,%x",
					timestamp,
					log_entries,
					(unsigned int)serial0, (unsigned int)serial1,
					hw_version,
					battery_voltage,
					log_get_percent_full(),
					NB_FW_VERSION);

		respondLine(info_response, strlen(info_response));

	}else if(_received[0] == 'j') { //general statistics.
		uint32_t dboot = 0; // days since boot
		uint32_t dowl = 0; // daily detection
		uint32_t wowl = 0; // weekly detection
		uint32_t mowl = 0; // monthly detection

		uint32_t devt = 0;
		uint32_t wevt = 0;
		uint32_t mevt = 0;

		system_status_fill_statistics(&dboot, &devt, &dowl, &wevt, &wowl, &mevt, &mowl);


		snprintf(info_response, INFO_LEN,
					"stat,%lu,%lu,%lu,%lu,%lu,%lu,%lu",
					dboot,
					devt,
					dowl,
					wevt,
					wowl,
					mevt,
					mowl);

		respondLine(info_response, strlen(info_response));

	}else if(_received[0] == 'f') { //factory reset: erase logs & stored calibrations.
		boot_factory_reset();

		respondLine("ok", strlen("ok"));

	}else if(_received[0] == 'e') { //erase logs, but keep calibration:
		log_erase_all();
		boot_create_backup(); // currently active calibration will get stored again.
		respondLine("ok", strlen("ok"));

	}else if(_received[0] == 't') { //synchronise time
		unsigned int j=0;
		while(_received[++j] != '\n' && j<MAX_LEN_RECEIVED_LINE);

		_received[j] = 0;

		long timestamp = atol(&(_received[1]));
		rtc_set_timestamp(timestamp);


		respondLine("ok", strlen("ok"));

	}else if(_received[0] == 'u') { //put into fw update mode

		respondLine("ok", strlen("ok"));
		HAL_Delay(10);

		boot_reboot_with_bootloader();
		// should not reach this line if reboot is successful:
		respondLine("error", strlen("error"));
	}else if(_received[0] == 'l') { //download next log chunk
		cli_download_next_log_chunk();
	}else if(_received[0] == '0') { //reset log counter to beginning
		total_printed_log_entry_iterator = 0;
		log_read_from_start(); // reset log read pointer to start of flash:
		respondLine("ok", strlen("ok"));
	}else if(_received[0] == 'k'){ // force backup state to flash
		// backup all variables and log.
		boot_create_backup();
		respondLine("ok", strlen("ok"));
	}else if(_received[0] == 's') {
		switch(_received[1])
		{
		case 'i': //turn on Sigfox module, but keep in standby. Reply with info string (ID, etc...)
			if(comm_get_info_string(info_response, INFO_LEN) == COMM_OK)
				respondLine(info_response, strlen(info_response));
			else
				respondLine("error", strlen("error"));
			break;
		case '0': //turn Sigfox CW OFF (for EMI test purposes)
			comm_emi_test_off();
			respondLine("SIGFOX off", strlen("SIGFOX off"));
			break;
		case '1': //turn Sigfox CW on at center frequency (for EMI test purposes)
			comm_emi_test_cw_on(SIGFOX_FREQ_DEFAULT);
			respondLine("SIGFOX CW 868.130MHz on", strlen("SIGFOX CW 868.130MHz on"));
			break;
		case '2': //turn Sigfox CW on at lower band frequency (for EMI test purposes)
			comm_emi_test_cw_on(SIGFOX_FREQ_LOWER);
			respondLine("SIGFOX CW 868.055MHz on", strlen("SIGFOX CW 868.130MHz on"));
			break;
		case '3': //turn Sigfox CW on at upper band frequency (for EMI test purposes)
			comm_emi_test_cw_on(SIGFOX_FREQ_UPPER);
			respondLine("SIGFOX CW 868.205MHz on", strlen("SIGFOX CW 868.130MHz on"));
			break;
		case '4': //turn Sigfox modulated on at center frequency (for EMI test purposes)
			comm_emi_test_pattern_on(SIGFOX_FREQ_DEFAULT);
			respondLine("SIGFOX modulated 868.130MHz on", strlen("SIGFOX modulated 868.130MHz on"));
			break;
		case '5': //turn Sigfox modulated on at lower band frequency (for EMI test purposes)
			comm_emi_test_pattern_on(SIGFOX_FREQ_LOWER);
			respondLine("SIGFOX modulated 868.055MHz on", strlen("SIGFOX modulated 868.130MHz on"));
			break;
		case '6': //turn Sigfox modulated on at upper band frequency (for EMI test purposes)
			comm_emi_test_pattern_on(SIGFOX_FREQ_UPPER);
			respondLine("SIGFOX modulated 868.205MHz on", strlen("SIGFOX modulated 868.130MHz on"));
			break;
		case 'c': //force send a SIGFOX calibration message
			comm_force_tx_once();
			comm_send_calibration();
			respondLine("ok", strlen("ok"));
			break;
		case 's': //force send a SIGFOX status message
			comm_force_tx_once();
			if(comm_send_status(COMM_UPLINK_ONLY, LOG_TO_MEM_AND_TX) == COMM_OK)
				respondLine("ok", strlen("ok"));
			else
				respondLine("error", strlen("error"));

			break;
		case 'l': // downlink status (new in updated firmware)
			comm_get_downlink_string(info_response, INFO_LEN);
			respondLine(info_response, strlen(info_response));
			break;
		default:
			respondLine("~", 1);
			break;
		}
	}else if(_received[0] == 'w' && _received[1] == 'o') { //return last measured offset value
		load_cell_get_offset_string(info_response, INFO_LEN);
		respondLine(info_response, strlen(info_response));
	}else if(_received[0] == 'w' && _received[1] == '1') { //return one sample measurement
		load_cell_get_weight_string(info_response, INFO_LEN);
		respondLine(info_response, strlen(info_response));
	}else if(_received[0] == 'w' && _received[1] == 'c') { //return calibration data
		calib_get_string(info_response, INFO_LEN);
		respondLine(info_response, strlen(info_response));
	}else if(_received[0] == 'w' && _received[1] == 'i') { //return lastcalibration info
		calib_get_info_string(info_response, INFO_LEN);
		respondLine(info_response, strlen(info_response));
	}else if(_received[0] == 'w' && _received[1] == 'j') { //return ongoing calibration info
		calib_get_ongoing_info_string(info_response, INFO_LEN);
		respondLine(info_response, strlen(info_response));
	}else if(_received[0] == 'w' && _received[1] == 'r') { //re-enter calibration mode
		if(load_cell_restart_calibration_mode() == LOAD_CELL_UNINIT)
			respondLine("Error: Load cell not initialised.", strlen("Error: Load cell not initialised."));
		else
			respondLine("Calibration mode started.", strlen("Calibration mode started."));
	}else if(_received[0] == 'w' && _received[1] == 'x') { //reset all calibration weight measurements
		calib_reset_ongoing();
		respondLine("ok", strlen("ok"));
	}else if(_received[0] == 'r' && _received[1] == 'i') { //return one sample RFID tag readout
		rfid_get_string(info_response, INFO_LEN);
		respondLine(info_response, strlen(info_response));
	// inputs for EMC tests:
	}else if(_received[0] == 'r' && _received[1] == '1') { //turn RFID field on
		rfid_on();
		respondLine("RFID 125kHz on", strlen("RFID 125kHz on"));
	}else if(_received[0] == 'r' && _received[1] == '0') { //turn RFID OFF
		rfid_off();
		respondLine("RFID 125kHz off", strlen("RFID 125kHz off"));
	}else if(_received[0] == 'x' ) { //request state change to leave wakeup state.
									// --> This allows user to perform calibration routine.
		nestbox_request_complete_startup();
//		respondLine("ok", strlen("ok")); --> done in main loop after state change
	}else{
		respondLine("~", 1);
	}
	clearReceived(); // todo-ok: only delete current command
}

#define CLI_LOG_BUF_LEN		(LOG_CONTENT_LENGTH + 3 + LOG_CRC_LENGTH+1)
void cli_download_next_log_chunk()
{
	/**
	 * - individual log entries are encoded into base64 strings terminated by a \n
	 * - log download is terminated by EOT character and newline: "0x04\n"
	 *
	 */
	char info_response[INFO_LEN];
	char buf[CLI_LOG_BUF_LEN]; // buffer for original log data

#ifdef CLI_ENCODE_B64
	char buf_b64[b64_encoded_size(CLI_LOG_BUF_LEN)]; // buffer for base64-encoded log data
#endif

	unsigned int this_chunk_printed_log_entry_iterator = 0;

	LOG_message_t log;

	// read first log entry
	LOG_entry_type_t next_entry_type = log_read_next_entry(&log);

	unsigned int log_len;

	while(this_chunk_printed_log_entry_iterator < LOG_ENTRIES_PER_CHUNK
			&& next_entry_type > LOG_END_OF_DATA)
	{
		// write chars that all log entries have in common:
		buf[1] = ':'; // separator

		switch(next_entry_type)
		{
		case LOG_SIGFOX_ENTRY:
			buf[0] = log.header[0];
			memcpy(&buf[2], (uint8_t*)&(log.msg), LOG_CONTENT_LENGTH);
			buf[2+LOG_CONTENT_LENGTH] = ':';
			memcpy(&buf[3+LOG_CONTENT_LENGTH], &(log.crc), LOG_CRC_LENGTH);

			log_len = LOG_CONTENT_LENGTH + 3 + LOG_CRC_LENGTH;
			break;
		default:
			log_len = 0;
		}

#ifdef CLI_ENCODE_B64
		// encode the log entry with Base64:
		unsigned int b64log_len = b64_encode(buf, log_len, buf_b64);

		buf_b64[b64log_len] = '\n'; //index b64log_len of this array is used by the \0 character (string delimiter).
#endif

		if(total_printed_log_entry_iterator % LOG_ENTRIES_PER_GROUP == 0)
		{
			memset(info_response, 0, INFO_LEN);
			snprintf(info_response, INFO_LEN, "l:%u\n", total_printed_log_entry_iterator);
			usbSafeTransmit(info_response, strlen(info_response));
		}

#ifdef CLI_ENCODE_B64
		//send the base64 converted log entry + newline character
		if(usbSafeTransmit(buf_b64, b64log_len+1) == USBD_OK)
		{
			// read next log entry:
			next_entry_type = log_read_next_entry(&log);
		}
		else
		{
			next_entry_type = LOG_ABORT; // aborts the main while loop
			break;
		}
#else
	// todo-ok if non-base64 data should be outputted

#endif

		// increase counter:
		total_printed_log_entry_iterator++;
		this_chunk_printed_log_entry_iterator++;
	}

	//end of log
	if(next_entry_type == LOG_END_OF_DATA)
	{
		char end_of_log[] = {0x04,'\n',0x0}; // send End of Transmission character

		usbSafeTransmit(end_of_log, strlen(end_of_log));
	}
	else
	{
		char end_of_log[] = {0x03,'\n',0x0}; // send End of Text character

		usbSafeTransmit(end_of_log, strlen(end_of_log));
	}
}

void cli_cdc_rx(uint8_t * data, uint8_t len){

	memset(_received, 0, MAX_LEN_RECEIVED_LINE);
	memcpy(_received, data, len);

	_received_len = len;

	cli_process_rx_IT();
}
