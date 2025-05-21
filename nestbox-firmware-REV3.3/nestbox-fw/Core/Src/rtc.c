/**
  ******************************************************************************
  * @file    rtc.c
  * @brief   This file provides code for the configuration
  *          of the RTC instances.
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

/* Includes ------------------------------------------------------------------*/
#include "rtc.h"

/* USER CODE BEGIN 0 */
#include <time.h>
/* USER CODE END 0 */

RTC_HandleTypeDef hrtc;

/* RTC init function */
void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  // the precompiler directive is used to deactivate the cubeIDE-generated code below.
#ifdef RTC_RESET_AT_INIT
  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};
  RTC_AlarmTypeDef sAlarm = {0};

  /* USER CODE BEGIN RTC_Init 1 */
#endif
  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  // the precompiler directive is used to deactivate the cubeIDE-generated code below.
#ifdef RTC_RESET_AT_INIT
  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0;
  sTime.Minutes = 0;
  sTime.Seconds = 0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 1;
  sDate.Year = 0;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enable the Alarm A
  */
  sAlarm.AlarmTime.Hours = 0;
  sAlarm.AlarmTime.Minutes = 0;
  sAlarm.AlarmTime.Seconds = 0;
  sAlarm.AlarmTime.SubSeconds = 0;
  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  sAlarm.AlarmDateWeekDay = 1;
  sAlarm.Alarm = RTC_ALARM_A;
  if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enable the WakeUp
  */
  if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 0, RTC_WAKEUPCLOCK_CK_SPRE_16BITS) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */
#else

  // todo: the wake-up timer may be required to initialise already at boot (?)
//  if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 0, RTC_WAKEUPCLOCK_CK_SPRE_16BITS) != HAL_OK)
//  {
//     Error_Handler();
//  }
#endif
  /* USER CODE END RTC_Init 2 */

}

void HAL_RTC_MspInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspInit 0 */

  /* USER CODE END RTC_MspInit 0 */
    /* RTC clock enable */
    __HAL_RCC_RTC_ENABLE();

    /* RTC interrupt Init */
    HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
    HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
  /* USER CODE BEGIN RTC_MspInit 1 */

  /* USER CODE END RTC_MspInit 1 */
  }
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspDeInit 0 */

  /* USER CODE END RTC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_RTC_DISABLE();

    /* RTC interrupt Deinit */
    HAL_NVIC_DisableIRQ(RTC_WKUP_IRQn);
    HAL_NVIC_DisableIRQ(RTC_Alarm_IRQn);
  /* USER CODE BEGIN RTC_MspDeInit 1 */

  /* USER CODE END RTC_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

#define RTC_MINIMUM_INITIALISED_YEAR	9 // factory reset time is (1.1.2010)
#define RTC_MINIMUM_VALID_YEAR			19 // factory reset time is (1.1.2010)

rtc_state_t rtc_get_state(RTC_TimeTypeDef* sTime, RTC_DateTypeDef* sDate)
{
	HAL_RTC_GetTime(&hrtc, sTime, RTC_FORMAT_BIN); // call this before RTC_GetDate.
	HAL_RTC_GetDate(&hrtc, sDate, RTC_FORMAT_BIN);

	if(sDate->Year > RTC_MINIMUM_VALID_YEAR)
		return RTC_VALID;
	if(sDate->Year > RTC_MINIMUM_INITIALISED_YEAR)
		return RTC_INIT;
	else
		return RTC_UNINIT;
}

HAL_StatusTypeDef rtc_set_utc(uint8_t Year,uint8_t Month,uint8_t Date, uint8_t Hours, uint8_t Minutes, uint8_t Seconds)
{
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;

    if ((hrtc.Instance->CR & RTC_CR_FMT) != 0U)
    {
    	// RTC is in AM/PM mode --> try re-init.
    	MX_RTC_Init();
    }

	sTime.Hours = Hours;
	sTime.Minutes = Minutes;
	sTime.Seconds = Seconds;

	HAL_StatusTypeDef resp1 = HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);

	sDate.Year = Year;
	sDate.Month = Month;
	sDate.Date = Date;
	sDate.WeekDay = 0; // important to set this, as its default value may overwrite the Year field in the RTC_DateRegister

	HAL_StatusTypeDef resp2 = HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

	if(resp1 == HAL_OK && resp2 == HAL_OK)
	{
		return HAL_OK;
	}
	else
	{
		return HAL_ERROR;
	}
}

HAL_StatusTypeDef rtc_get_utc(uint8_t* Year,uint8_t* Month,uint8_t* Date, uint8_t* Hours, uint8_t* Minutes, uint8_t* Seconds)
{
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;

	if(rtc_get_state(&sTime, &sDate) == RTC_UNINIT)
	{
		return HAL_ERROR;
	}

	if ((hrtc.Instance->CR & RTC_CR_FMT) != 0U)
	{
		// RTC is in AM/PM mode --> try re-init.
		MX_RTC_Init();

		return HAL_ERROR;
	}


//	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	*Hours = sTime.Hours;
	*Minutes = sTime.Minutes;
	*Seconds = sTime.Seconds;

	/* You must call HAL_RTC_GetDate() after HAL_RTC_GetTime() to unlock the values
	 * in the higher-order calendar shadow registers to ensure consistency between
	 * the time and date values.
	 */

//	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
	*Year = sDate.Year;
	*Month = sDate.Month;
	*Date = sDate.Date;

	return HAL_OK;
}

void rtc_set_timestamp(uint32_t epoch)
{
	time_t rawtime = epoch;
	struct tm *info;

   /* Get GMT time */
   info = gmtime(&rawtime);

   rtc_set_utc(info->tm_year - 100, // tm_year is (<currentyear> - 1900)
		   	   info->tm_mon + 1,	// tm_mon: 0 = jan
			   info->tm_mday,
			   info->tm_hour,
			   info->tm_min,
			   info->tm_sec);
}

uint32_t rtc_get_timestamp()
{
	struct tm t;

	uint8_t Year;
	uint8_t Month;
	uint8_t Date;
	uint8_t Hours;
	uint8_t Minutes;
	uint8_t Seconds;

	if(rtc_get_utc(&Year,&Month,&Date,&Hours,&Minutes,&Seconds) == HAL_OK)
	{
		t.tm_year = Year+100;  // Year - 1900
		t.tm_mon = Month-1;    // Month, where 0 = jan
		t.tm_mday = Date;      // Day of the month
		t.tm_hour = Hours;
		t.tm_min = Minutes;
		t.tm_sec = Seconds;
		t.tm_isdst = 0;        // Is DST on? 1 = yes, 0 = no, -1 = unknown
		time_t t_of_day = mktime(&t);

		return t_of_day;
	}
	else
	{
		return HAL_GetTick()/1000;
	}


}

uint64_t rtc_get_timestamp_ms()
{
	uint64_t ms = rtc_get_timestamp(); // convert to uint64 before multiplying !!
	ms = ms*1000;
	ms += (HAL_GetTick() % 1000);

	return ms;
}

void rtc_reset_date_time()
{
	rtc_set_timestamp(1262346000); // Friday, January 1, 2010 11:40:00   GMT
}

/***** SPECTRACE Periodic wake-up interrupt configuration (multiples of 1 Hz) ********/
HAL_StatusTypeDef rtc_start_periodic_timer(uint32_t period_seconds)
{
	 /* Configure the RTC WakeUp Clock source: CK_SPRE (1Hz) */
	__HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&hrtc, RTC_FLAG_WUTF);
	return HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, period_seconds - 1, RTC_WAKEUPCLOCK_CK_SPRE_16BITS);
}

HAL_StatusTypeDef rtc_stop_periodic_timer()
{
	/* Disable the RTC WakeUp clock */
	return HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
}

HAL_StatusTypeDef rtc_set_alarm(uint8_t hour, uint8_t minute)
{
	 /* Configure the RTC Alarm */
	RTC_AlarmTypeDef alarm;
	alarm.Alarm = RTC_ALARM_A;
	alarm.AlarmTime.Hours = hour;
	alarm.AlarmTime.Minutes = minute;
	alarm.AlarmTime.Seconds = 0;
	alarm.AlarmTime.SubSeconds = 0;
	alarm.AlarmTime.TimeFormat = RTC_HOURFORMAT_24;

	alarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY; //If a field is masked, then that won't be compared when checking alarm date. So when you mask SECONDS, then only the DAY, HOUR and MINUTE fields will be compared
	alarm.SubSeconds = 0;
	alarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE; // SS[14:0] are compared and must match.
	alarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_WEEKDAY;
	alarm.AlarmDateWeekDay = RTC_WEEKDAY_MONDAY;


	// All the parameters need to be set because they get written into the alarm config register!!!
	/*
	 * See code from HAL below:
	 *
	 *   tmpreg = (((uint32_t)RTC_ByteToBcd2(sAlarm->AlarmTime.Hours) << RTC_ALRMAR_HU_Pos) | \
              ((uint32_t)RTC_ByteToBcd2(sAlarm->AlarmTime.Minutes) << RTC_ALRMAR_MNU_Pos) | \
              ((uint32_t)RTC_ByteToBcd2(sAlarm->AlarmTime.Seconds) << RTC_ALRMAR_SU_Pos) | \
              ((uint32_t)(sAlarm->AlarmTime.TimeFormat) << RTC_ALRMAR_PM_Pos) | \
              ((uint32_t)RTC_ByteToBcd2(sAlarm->AlarmDateWeekDay) << RTC_ALRMAR_DU_Pos) | \
              ((uint32_t)sAlarm->AlarmDateWeekDaySel) | \
              ((uint32_t)sAlarm->AlarmMask));
	 *
	 */

	HAL_RTC_DeactivateAlarm(&hrtc, RTC_ALARM_A);

	return HAL_RTC_SetAlarm_IT(&hrtc, &alarm, RTC_FORMAT_BIN);
}

void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
	// things needed to be performed at wake up are handled in the system_sleep functions.
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
