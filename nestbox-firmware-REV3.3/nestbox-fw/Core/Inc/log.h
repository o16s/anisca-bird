/*
 * log.h
 *
 *  Created on: Jan 18, 2022
 *      Author: raffael
 */

#ifndef INC_LOG_H_
#define INC_LOG_H_

/*
 * log.h
 *
 *  Created on: 3 Sep 2020
 *      Author: sam
 */

#include "stdlib.h"
#include "stdio.h"
#include "main.h"
#include "log_types.h"
#include "GD5F1GQ4.h"

#define LOG_MAX_DATA_LOSS_ENTRIES	200 // maximum number of logs that can be lost for an unexpected power loss/reboot

// Backup register assignments:
typedef enum LOG_BKP_REG_LIST{
// CALIB:
LOG_BKP_REG_CALIB_SLOPE	= 0, 	//GD5F1GQ4_BKP_DR0,
LOG_BKP_REG_CALIB_INTERCEPT, 	//= GD5F1GQ4_BKP_DR1,
LOG_BKP_REG_CALIB_R2, 		//= GD5F1GQ4_BKP_DR2,
LOG_BKP_REG_CALIB_TEMP, 	//= GD5F1GQ4_BKP_DR3,
LOG_BKP_REG_CALIB_INFO,		//= GD5F1GQ4_BKP_DR4,
LOG_BKP_REG_CALIB_TIME,		//= GD5F1GQ4_BKP_DR5,
LOG_BKP_REG_CALIB_PW,		//= GD5F1GQ4_BKP_DR6,
LOG_BKP_REG_CALIB_CHKSUM,	//= GD5F1GQ4_BKP_DR7,

// Log entries counter:
LOG_BKP_REG_LOG_ENTRIES,//	= GD5F1GQ4_BKP_DR8,


} LOG_BKP_REG_t;

typedef enum LOG_entry_type
{
	LOG_ABORT = -1,
	LOG_END_OF_DATA = 0,
	LOG_SIGFOX_ENTRY,
} LOG_entry_type_t;

LogFlash_State_t log_write_message(uint8_t* msg, unsigned int length);
LOG_entry_type_t log_read_next_entry(LOG_message_t* msg);

int log_read_all(uint8_t* log_buffer, int length);
void log_read_from_start();

int log_get_counter(); // get number of log entries
int log_get_new_logs_since_backup_counter(); // get number of log entries that are not yet persistent on flash

void log_init();
void log_erase_all();

uint8_t log_is_backup_valid();
float log_get_percent_full();

void log_write_backup_register(LOG_BKP_REG_t reg, uint32_t value);
uint32_t log_read_backup_register(LOG_BKP_REG_t reg);

void log_backup_state();



#endif /* INC_LOG_H_ */
