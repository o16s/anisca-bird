/*
 * boot.c
 *
 *  Created on: Jan 18, 2022
 *      Author: raffael
 */



#include <boot.h>
#include "rtc.h"
#include "log.h"
#include "system.h"
#include "calibration.h"
#include "load_cell.h"

static boot_reset_cause_t reset_cause;
//static ioSYS_STATE_t boot_flags;

boot_reset_cause_t boot_get_last_reset_cause()
{
	return reset_cause;
}

void boot_fetch_reset_cause()
{
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST))
    {
        reset_cause = BOOT_RESET_CAUSE_LOW_POWER_RESET;
    }
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST))
    {
        reset_cause = BOOT_RESET_CAUSE_SOFTWARE_RESET; // This reset is induced by calling the ARM CMSIS `NVIC_SystemReset()` function!
    }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_BORRST))
    {
        reset_cause = BOOT_RESET_CAUSE_POWER_ON_POWER_DOWN_RESET;
    }
    else
    {
    	reset_cause = BOOT_RESET_CAUSE_OTHER;
    }

//    boot_flags |= RESET_EVENT;

    // Clear all the reset flags or else they will remain set during future resets until system power is fully removed.
    __HAL_RCC_CLEAR_RESET_FLAGS();
}

HAL_StatusTypeDef boot_select_boot_from_flash()
{
	// Read all flash option Bytes:
	FLASH_OBProgramInitTypeDef pOBInit;
	HAL_FLASHEx_OBGetConfig(&pOBInit);

	// check if option bits are configured in boot-from-bootloader state.
	if(!(pOBInit.USERConfig & FLASH_OPTR_nBOOT0))
	{
		// --> BOOT0 = 1 --> reconfigure the option bits and generate system reset.
		HAL_Delay(1000);

		// Define that only the USERConfig shall be modified (i.e. FLASH->OPTR):
		pOBInit.OptionType = OPTIONBYTE_USER;
		// Define that nSWBOOT0 and nBOOT0 bits in OPTR shall be modified:
		pOBInit.USERType = OB_USER_nSWBOOT0 + OB_USER_nBOOT0;
		// Clear nSWBOOT0 and nBOOT0 bits:
		pOBInit.USERConfig = OB_BOOT0_FROM_OB + OB_BOOT0_SET;

		// Unlock FLASH and FLASH Option Bytes write access
		HAL_StatusTypeDef status = HAL_FLASH_Unlock();
		if(status == HAL_OK)
		{
			status = HAL_FLASH_OB_Unlock();
			if(status == HAL_OK)
			{
				// Write the configuration to the registers in flash.
				status = HAL_FLASHEx_OBProgram(&pOBInit);
				if(status == HAL_OK)
				{
					status = HAL_FLASH_OB_Launch(); // will trigger a reboot
				}
			}
		}
		return status; // statement will not be reached
	}
	else
	{
		HAL_Delay(1000);

		return HAL_OK;
	}
}


HAL_StatusTypeDef boot_reboot_with_bootloader()
{
	// Read all flash option Bytes:
	FLASH_OBProgramInitTypeDef pOBInit;
	HAL_FLASHEx_OBGetConfig(&pOBInit);

	// Define that only the USERConfig shall be modified (i.e. FLASH->OPTR):
	pOBInit.OptionType = OPTIONBYTE_USER;
	// Define that nSWBOOT0 and nBOOT0 bits in OPTR shall be modified:
	pOBInit.USERType = OB_USER_nSWBOOT0 + OB_USER_nBOOT0;
	// Clear nSWBOOT0 and nBOOT0 bits:
	pOBInit.USERConfig = OB_BOOT0_FROM_OB + OB_BOOT0_RESET;

	// Unlock FLASH and FLASH Option Bytes write access
	HAL_StatusTypeDef status = HAL_FLASH_Unlock();
	if(status == HAL_OK)
	{
		status = HAL_FLASH_OB_Unlock();
		if(status == HAL_OK)
		{
			// Write the configuration to the registers in flash.
			status = HAL_FLASHEx_OBProgram(&pOBInit);
			if(status == HAL_OK)
			{
				// Apply the configuration. This will generate a system reset and restart from the STM32 bootloader:
				return HAL_FLASH_OB_Launch(); //Note: this must be called right after programming (while Flash is still unlocked).

				// function should never return, as it generates reset
			}
			HAL_FLASH_OB_Lock();
		}
		HAL_FLASH_Lock();
	}

	return status;

}


void boot_create_backup()
{
	// backup rtc values
//	rtc_backup_date_time();

	// todo-ok: backup other needed data (offset, ...)
	calib_backup();


	log_backup_state();

}

void boot_restore_backup()
{
	// restore rtc values, encryption keys, etc...
	log_init();

	// todo-ok: restore needed data (calibration, offset)
	calib_restore();
}

void boot_factory_reset()
{
	// reset all flash content. do not generate MCU reset in order to keep USB connection alive
	log_erase_all();

	rtc_reset_date_time();

	boot_restore_backup(); // as log has been erased, the restore function will initialize default values.

	load_cell_init(); // this reinitializes the load cell and activates calibration mode.

	// todo-ok: reset comm state, etc...

	reset_cause = BOOT_RESET_CAUSE_FACTORY_RESET;
}


//ioSYS_STATE_t io_get_and_clear_system_flags(){
//	SPEC_POWER_usb_connected_t charging_state = SPEC_POWER_set_usb_connected(HAL_GPIO_ReadPin(USB_PGOODN_GPIO_Port, USB_PGOODN_Pin));
//
//	ioSYS_STATE_t retflags = boot_flags;
//	// clear local boot flags:
//	boot_flags = 0;
//
//	if(charging_state == USB_CONNECTED)
//	{
//		return (retflags | CHARGING);
//	}
//	else
//	{
//		if(io_get_last_battery_voltage()<VBAT_EMPTY_MV)
//			return (retflags | DISCHARGED);
//		else
//			return (retflags | NOT_CHARGING);
//	}
//}
