/*
 * system.h
 *
 *  Created on: 16 Sep 2021
 *      Author: raffael
 */

#ifndef INC_SYSTEM_H_
#define INC_SYSTEM_H_

#include <stdint.h>

#include "comm.h"

#define SYSTEM_POWER_DOWN	3
#define SYSTEM_INFO_INVALID	1
#define SYSTEM_OK			0

typedef struct SystemStatus
{
	uint16_t vbat_mv;
	uint8_t	soc;
	uint8_t memory_used;
	uint8_t error_flags;
	uint8_t status_unused;
	uint16_t days_since_boot;
	uint32_t events_with_id;	// True (non-capped) number of events with successful RFID detection since last status update.
	uint32_t events_unknown_id;	// True number of events with UN-successful RFID detection since last status update.


} system_status_t;


typedef enum SYS_usb_connected_enum{
	USB_DISCONNECTED = 0,
	USB_CONNECTED = 1,
	USB_DETECT_PIN_DISABLED = 2, // use this option while the pin is used as output to drive LED or buzzer.
} SYS_usb_connected_t;

SYS_usb_connected_t system_get_usb_connection_state();
int system_disable_usb_connection_state_pin();
void system_enable_usb_connection_state_pin();

void system_update_usb_connection_state_blocking();

void system_increment_events_with_id();
void system_increment_events_unknown_id();


int system_init();

void system_status_fill_statistics(uint32_t* dboot, uint32_t* devt, uint32_t* dowl, uint32_t* wevt, uint32_t* wowl, uint32_t* mevt, uint32_t* mowl);
void system_status_fill_heartbeat(comm_heartbeat_t* heartbeat, int reset_events_counter);
int system_info_invalid();

int system_measure_status_at_boot();
int system_measure_status(unsigned int vbat_limit);
int system_time_to_sleep();

void system_daytime_sleep();
void system_shutdown(); // to be called upon low battery, will turn off everything
void system_sleep_IT(unsigned int seconds); // puts device to sleep until next RTC wakeup event (i.e. max. 1 second)

#endif /* INC_SYSTEM_H_ */
