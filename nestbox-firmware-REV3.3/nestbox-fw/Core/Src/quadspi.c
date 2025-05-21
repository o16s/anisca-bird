/**
  ******************************************************************************
  * @file    quadspi.c
  * @brief   This file provides code for the configuration
  *          of the QUADSPI instances.
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
#include "quadspi.h"

/* USER CODE BEGIN 0 */
#include "spi_nand.h"
/* USER CODE END 0 */

QSPI_HandleTypeDef hqspi;

/* QUADSPI init function */
void MX_QUADSPI_Init(void)
{

  /* USER CODE BEGIN QUADSPI_Init 0 */

  /* USER CODE END QUADSPI_Init 0 */

  /* USER CODE BEGIN QUADSPI_Init 1 */

  /* USER CODE END QUADSPI_Init 1 */
  hqspi.Instance = QUADSPI;
  hqspi.Init.ClockPrescaler = 2;
  hqspi.Init.FifoThreshold = 1;
  hqspi.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_NONE;
  hqspi.Init.FlashSize = 28;
  hqspi.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_1_CYCLE;
  hqspi.Init.ClockMode = QSPI_CLOCK_MODE_0;
  hqspi.Init.FlashID = QSPI_FLASH_ID_1;
  hqspi.Init.DualFlash = QSPI_DUALFLASH_DISABLE;
  if (HAL_QSPI_Init(&hqspi) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN QUADSPI_Init 2 */

  /* USER CODE END QUADSPI_Init 2 */

}

void HAL_QSPI_MspInit(QSPI_HandleTypeDef* qspiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(qspiHandle->Instance==QUADSPI)
  {
  /* USER CODE BEGIN QUADSPI_MspInit 0 */

  /* USER CODE END QUADSPI_MspInit 0 */
    /* QUADSPI clock enable */
    __HAL_RCC_QSPI_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**QUADSPI GPIO Configuration
    PA2     ------> QUADSPI_BK1_NCS
    PA3     ------> QUADSPI_CLK
    PA6     ------> QUADSPI_BK1_IO3
    PA7     ------> QUADSPI_BK1_IO2
    PB0     ------> QUADSPI_BK1_IO1
    PB1     ------> QUADSPI_BK1_IO0
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_QUADSPI;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_QUADSPI;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN QUADSPI_MspInit 1 */

  /* USER CODE END QUADSPI_MspInit 1 */
  }
}

void HAL_QSPI_MspDeInit(QSPI_HandleTypeDef* qspiHandle)
{

  if(qspiHandle->Instance==QUADSPI)
  {
  /* USER CODE BEGIN QUADSPI_MspDeInit 0 */

  /* USER CODE END QUADSPI_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_QSPI_CLK_DISABLE();

    /**QUADSPI GPIO Configuration
    PA2     ------> QUADSPI_BK1_NCS
    PA3     ------> QUADSPI_CLK
    PA6     ------> QUADSPI_BK1_IO3
    PA7     ------> QUADSPI_BK1_IO2
    PB0     ------> QUADSPI_BK1_IO1
    PB1     ------> QUADSPI_BK1_IO0
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_6|GPIO_PIN_7);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_0|GPIO_PIN_1);

  /* USER CODE BEGIN QUADSPI_MspDeInit 1 */

  /* USER CODE END QUADSPI_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */



void qspi_DeInit()
{
	HAL_QSPI_DeInit(&hqspi);
}

HAL_StatusTypeDef qspi_command(QSPI_CommandTypeDef* sCommand)
{
	if(hqspi.State == HAL_QSPI_STATE_RESET)
	{
		nb_flash_power_on_blocking();
	}
	return HAL_QSPI_Command(&hqspi, sCommand, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);
}

HAL_StatusTypeDef qspi_transmit(uint8_t *pData)
{
	if(hqspi.State == HAL_QSPI_STATE_RESET)
	{
		nb_flash_power_on_blocking();
	}
	return HAL_QSPI_Transmit(&hqspi, pData, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);
}

HAL_StatusTypeDef qspi_receive(uint8_t *pData)
{
	if(hqspi.State == HAL_QSPI_STATE_RESET)
	{
		nb_flash_power_on_blocking();
	}
	return HAL_QSPI_Receive(&hqspi, pData, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);
}

HAL_StatusTypeDef qspi_autoPolling(QSPI_CommandTypeDef* sCommand, QSPI_AutoPollingTypeDef* sConfig)
{
	if(hqspi.State == HAL_QSPI_STATE_RESET)
	{
		nb_flash_power_on_blocking();
	}
	return HAL_QSPI_AutoPolling(&hqspi, sCommand, sConfig, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);
}


/////////// POWER ON/OFF FUNCTIONS /////////////////

void nb_flash_power_on_blocking()
{
	HAL_GPIO_WritePin(FLASH_POWER_ON_N_GPIO_Port, FLASH_POWER_ON_N_Pin, 0);

	MX_QUADSPI_Init();

	HAL_Delay(1); // delay after FLASH_POWER_ON_N is asserted is needed to start up flash!

	int timeout = 0;
	while(spi_nand_init() != SPI_NAND_RET_OK)
	{
		if(timeout > 3)
			break;

		nb_flash_power_off();
		HAL_Delay(10);

		HAL_GPIO_WritePin(FLASH_POWER_ON_N_GPIO_Port, FLASH_POWER_ON_N_Pin, 0);

		MX_QUADSPI_Init();

		HAL_Delay(1);

		timeout++;
	}
}

void nb_flash_power_off()
{
	qspi_DeInit();

	HAL_GPIO_WritePin(FLASH_POWER_ON_N_GPIO_Port, FLASH_POWER_ON_N_Pin, 1);

}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
