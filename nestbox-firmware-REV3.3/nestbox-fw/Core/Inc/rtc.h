/**
  ******************************************************************************
  * @file    rtc.h
  * @brief   This file contains all the function prototypes for
  *          the rtc.c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RTC_H__
#define __RTC_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
typedef enum rtc_valid_retval{
	RTC_UNINIT = 0,
	RTC_INIT,
	RTC_VALID
} rtc_state_t;
/* USER CODE END Includes */

extern RTC_HandleTypeDef hrtc;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_RTC_Init(void);

/* USER CODE BEGIN Prototypes */

/* Expected time format:
 * - Year: [0, 99]
 * - Month: [1,12]
 * - Date: [1,31]
 * - Hours: [0,23]
 * - Minutes: [0,59]
 * - Seconds: [0,59]
 */
HAL_StatusTypeDef rtc_set_utc(uint8_t Year,uint8_t Month,uint8_t Date, uint8_t Hours, uint8_t Minutes, uint8_t Seconds);
HAL_StatusTypeDef rtc_get_utc(uint8_t* Year,uint8_t* Month,uint8_t* Date, uint8_t* Hours, uint8_t* Minutes, uint8_t* Seconds);

void rtc_set_timestamp(uint32_t epoch);
uint32_t rtc_get_timestamp();
uint64_t rtc_get_timestamp_ms();

rtc_state_t rtc_get_state(RTC_TimeTypeDef* sTime, RTC_DateTypeDef* sDate);

/* Backup and restore RTC values in case of system reset.
 *
 * NOTE: this does not currently write the values into non-volatile memory. A power loss will reset the backup values too.
 *
 */
void rtc_backup_date_time();
uint32_t rtc_restore_date_time();


/* Set an alarm that will wake up the system every day at the same time */
HAL_StatusTypeDef rtc_set_alarm(uint8_t hour, uint8_t minute);


/* Start a periodic wake-up timer generating a PERIODIC_EVENT. The period_seconds is the time interval between wakeups in seconds. */
HAL_StatusTypeDef rtc_start_periodic_timer(uint32_t period_seconds);

/* Stop the periodic timer */
HAL_StatusTypeDef rtc_stop_periodic_timer();

/* sets the active RTC values to epoch = 0 */
void rtc_reset_date_time();

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __RTC_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
