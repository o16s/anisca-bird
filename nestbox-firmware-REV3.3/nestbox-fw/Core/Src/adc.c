/**
  ******************************************************************************
  * @file    adc.c
  * @brief   This file provides code for the configuration
  *          of the ADC instances.
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
#include "adc.h"

/* USER CODE BEGIN 0 */
#include "nestbox.h"
/* USER CODE END 0 */

ADC_HandleTypeDef hadc1;

/* ADC1 init function */
void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV128;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_9;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspInit 0 */

  /* USER CODE END ADC1_MspInit 0 */
    /* ADC1 clock enable */
    __HAL_RCC_ADC_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**ADC1 GPIO Configuration
    PA4     ------> ADC1_IN9
    */
    GPIO_InitStruct.Pin = VBAT_ADC9_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(VBAT_ADC9_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN ADC1_MspInit 1 */

  /* USER CODE END ADC1_MspInit 1 */
  }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{

  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspDeInit 0 */

  /* USER CODE END ADC1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ADC_CLK_DISABLE();

    /**ADC1 GPIO Configuration
    PA4     ------> ADC1_IN9
    */
    HAL_GPIO_DeInit(VBAT_ADC9_GPIO_Port, VBAT_ADC9_Pin);

  /* USER CODE BEGIN ADC1_MspDeInit 1 */

  /* USER CODE END ADC1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

void adc_DeInit()
{
	HAL_ADC_DeInit(&hadc1);
}

static int last_vbat = -1;

#define ADC_EMPIRICAL_SCALING 	1.0f //1.3822f //todo-ok find out why ADC value is incorrect and remove scaling factor

static int adc_measure_battery_voltage()
{
	uint32_t tmpval = 0;
    int vbat = 0;

	HAL_GPIO_WritePin(BAT_TEST_EN_GPIO_Port, BAT_TEST_EN_Pin, 1);
	HAL_Delay(10); // wait for value to settle (todo-ok: confirm settling time by measurement)

    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK)
    {
		tmpval = HAL_ADC_GetValue(&hadc1);

		HAL_ADC_Stop(&hadc1);
		HAL_GPIO_WritePin(BAT_TEST_EN_GPIO_Port, BAT_TEST_EN_Pin, 0);

		tmpval = 6600*tmpval; // 3300*(100+100)*tmpval/100;
		tmpval = (float)tmpval * ADC_EMPIRICAL_SCALING;
		vbat = tmpval/4096;
		//battery voltage resistive divider: 162k + 162k
		//ADC resolution: 12bits
		//VDDA = VREF+ = 3300 mV

		if(last_vbat < 0)
			last_vbat = vbat;
		else
			last_vbat = (last_vbat*9 + vbat)/10; // rolling average
      return 0;

    }

    HAL_ADC_Stop(&hadc1);
	HAL_GPIO_WritePin(BAT_TEST_EN_GPIO_Port, BAT_TEST_EN_Pin, 0);

    return -1;
}

int io_battery_level()
{
	adc_measure_battery_voltage();

	int retval = 0;
	retval = (last_vbat-VBAT_EMPTY_MV)*100/(VBAT_FULL_MV-VBAT_EMPTY_MV);
	if(retval<0)
		retval = 0;
	else if(retval>100)
		retval = 100;
	return retval;
}

// NOTE: these functions must be called after a io_battery_level was called!
unsigned int io_get_last_battery_voltage(){
	if(last_vbat > 0)
		return last_vbat;
	else
		return 0;
}
//
//int io_get_last_battery_soc(){
//	int retval = 0;
//	retval = (last_vbat-VBAT_EMPTY_MV)*100/(VBAT_FULL_MV-VBAT_EMPTY_MV);
//	if(retval<0)
//		retval = 0;
//	else if(retval>100)
//		retval = 100;
//	return retval;
//}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
