/*
 * boot.h
 *
 *  Created on: Jan 18, 2022
 *      Author: raffael
 */

#ifndef INC_BOOT_H_
#define INC_BOOT_H_


#include "main.h"

typedef enum boot_reset_causes {
	BOOT_RESET_CAUSE_OTHER = 0,
	BOOT_RESET_CAUSE_POWER_ON_POWER_DOWN_RESET,
	BOOT_RESET_CAUSE_SOFTWARE_RESET,
	BOOT_RESET_CAUSE_LOW_POWER_RESET,
	BOOT_RESET_CAUSE_FACTORY_RESET // this is NOT an MCU reset
} boot_reset_cause_t;

/* returns the last reset cause. to be called in INITIALISING state in order to know how peripherals should be initialised */
boot_reset_cause_t boot_get_last_reset_cause();

/* reads reset cause from RCC register. to be called at start of main() */
void boot_fetch_reset_cause();

/* Call this function at boot to not make the MCU boot from internal memory after power-off: */
HAL_StatusTypeDef boot_select_boot_from_flash();

/* Call this function to put MCU in bootloader mode */
HAL_StatusTypeDef boot_reboot_with_bootloader();

/* Restore rtc values, calibration and log state */
void boot_restore_backup();

/* Backup rtc values, calibration and log state */
void boot_create_backup();

/* Erase all log and settings */
void boot_factory_reset();


#endif /* INC_BOOT_H_ */
