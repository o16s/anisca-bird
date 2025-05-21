/*
 * log.c
 *
 *  Created on: Jan 18, 2022
 *      Author: raffael
 */

#include "log.h"
#include <string.h>
#include "crc.h"
#include "rtc.h"

#ifdef USE_GD5F1GQ4_B
#error "wrong Flash type defined!"
#endif

/* CRC length after the data type tag */

static int log_entries_counter = 0;
static int last_backed_up_log_entry = 0;

LogFlash_State_t log_write_message(uint8_t* msg, unsigned int length)
{
	if(length > LOG_MSG_LENGTH)
		return LOG_FLASH_ERROR;

	LOG_message_t log;

	// get timestamp:
	log.timestamp = rtc_get_timestamp();

	// copy message:
	memcpy(log.msg, msg, length);
	if(length < LOG_MSG_LENGTH)
	{
		memset(&(log.msg[length]), 0, LOG_MSG_LENGTH - length);
	}

	// add header:
	memset(log.header, LOG_HEADER_CHAR, LOG_HEADER_LENGTH);

	// compute CRC on timestamp + message:
	log.crc = crc_compute((uint8_t*)&(log.msg), LOG_CONTENT_LENGTH);

	// write entire log struct to flash
	LogFlash_State_t res = GD5F1GQ4_log_write((uint8_t*)&log, LOG_STRUCT_LENGTH);

	if(res == LOG_FLASH_OK)
		log_entries_counter++; // increment log entry counter

	return res;
}

LOG_entry_type_t log_read_next_entry(LOG_message_t* log)
{
	int unreadable_bytes = 0;

	while(GD5F1GQ4_log_read((uint8_t*)log, LOG_STRUCT_LENGTH) == LOG_STRUCT_LENGTH)
	{
		// check header:
		int header_ok = 1;
		for(int i=0; i<LOG_HEADER_LENGTH; i++)
		{
			if(log->header[0] != LOG_HEADER_CHAR)
			{
				header_ok = 0;
				break;
			}
		}

		if(header_ok)
		{
			// check CRC (TODO-NEXT: crc computation does not match the previous computation even with identical data:
//			if(crc_compute((uint8_t*)&(log->msg), LOG_CONTENT_LENGTH) == log->crc)
				return LOG_SIGFOX_ENTRY;

	//		else
	//			return LOG_END_OF_DATA;
		}

		unreadable_bytes += 1;

		// in case a byte shift exists, move ahead by one byte only:
		for(int i = 0; i<LOG_STRUCT_LENGTH-1; i++)
		{
			// shift bytes in log struct:
			((uint8_t*)log)[i] = ((uint8_t*)log)[i+1];
		}
		// read additional byte:
		if(GD5F1GQ4_log_read(&(((uint8_t*)log)[LOG_STRUCT_LENGTH-1]), 1) != 1)
			return LOG_END_OF_DATA;

	}
	return LOG_END_OF_DATA;
}

int log_read_all(uint8_t* log_buffer, int length)
{
	return GD5F1GQ4_log_read(log_buffer, length);
}

void log_read_from_start()
{
	GD5F1GQ4_log_goto_first_read_addr();
}

int log_get_counter()
{
	return log_entries_counter;
}

int log_get_new_logs_since_backup_counter()
{
	return log_entries_counter - last_backed_up_log_entry;
}

void log_erase_all()
{
	log_entries_counter = 0;
	GD5F1GQ4_log_erase_and_reset();
}

void log_write_backup_register(LOG_BKP_REG_t reg, uint32_t value)
{
	GD5F1GQ4_write_backup_register(reg, value);
}

uint32_t log_read_backup_register(LOG_BKP_REG_t reg)
{
	return GD5F1GQ4_read_backup_register(reg);
}

uint8_t log_is_backup_valid()
{
	return GD5F1GQ4_is_backup_valid();
}

float log_get_percent_full()
{
	float percent = GD5F1GQ4_get_used_number_of_sectors();
	uint32_t capacity = GD5F1GQ4_get_total_number_of_sectors();
	if(capacity > 0)
		percent = (percent)/(float)(capacity);
	else
		percent = 0.0; // todo: fix flash library when initializing an empty map.
	return percent*100.0;
}


void log_init()
{
	//needs to be called before any flash operation
	if(GD5F1GQ4_init() == LOG_FLASH_DATA_RESTORED)
	{
		// restore log entry counter:
		log_entries_counter = GD5F1GQ4_read_backup_register(LOG_BKP_REG_LOG_ENTRIES);
		last_backed_up_log_entry = log_entries_counter;
	}
}

void log_backup_state()
{
	// backup log entry counter:
	GD5F1GQ4_write_backup_register(LOG_BKP_REG_LOG_ENTRIES, log_entries_counter);

	// write temporary backup registers and flash-related variables to external flash:
	GD5F1GQ4_backup_state();

	last_backed_up_log_entry = log_entries_counter;
}
