/**
  ******************************************************************************
  * @file    spi.c
  * @brief   This file provides code for the configuration
  *          of the SPI instances.
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
#include "spi.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

SPI_HandleTypeDef hspi3;

/* SPI3 init function */
void MX_SPI3_Init(void)
{

  /* USER CODE BEGIN SPI3_Init 0 */

  /* USER CODE END SPI3_Init 0 */

  /* USER CODE BEGIN SPI3_Init 1 */

  /* USER CODE END SPI3_Init 1 */
  hspi3.Instance = SPI3;
  hspi3.Init.Mode = SPI_MODE_MASTER;
  hspi3.Init.Direction = SPI_DIRECTION_2LINES;
  hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi3.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi3.Init.NSS = SPI_NSS_SOFT;
  hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi3.Init.CRCPolynomial = 7;
  hspi3.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi3.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  if (HAL_SPI_Init(&hspi3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI3_Init 2 */

  /* USER CODE END SPI3_Init 2 */

}

void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(spiHandle->Instance==SPI3)
  {
  /* USER CODE BEGIN SPI3_MspInit 0 */

  /* USER CODE END SPI3_MspInit 0 */
    /* SPI3 clock enable */
    __HAL_RCC_SPI3_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**SPI3 GPIO Configuration
    PB3 (JTDO-TRACESWO)     ------> SPI3_SCK
    PB4 (NJTRST)     ------> SPI3_MISO
    PB5     ------> SPI3_MOSI
    */
    GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* SPI3 interrupt Init */
    HAL_NVIC_SetPriority(SPI3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(SPI3_IRQn);
  /* USER CODE BEGIN SPI3_MspInit 1 */

  /* USER CODE END SPI3_MspInit 1 */
  }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{

  if(spiHandle->Instance==SPI3)
  {
  /* USER CODE BEGIN SPI3_MspDeInit 0 */

  /* USER CODE END SPI3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SPI3_CLK_DISABLE();

    /**SPI3 GPIO Configuration
    PB3 (JTDO-TRACESWO)     ------> SPI3_SCK
    PB4 (NJTRST)     ------> SPI3_MISO
    PB5     ------> SPI3_MOSI
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5);

    /* SPI3 interrupt Deinit */
    HAL_NVIC_DisableIRQ(SPI3_IRQn);
  /* USER CODE BEGIN SPI3_MspDeInit 1 */

  /* USER CODE END SPI3_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* From old nestbox fw:
 *
 *         spiParams.frameFormat = SPI_POL0_PHA1;
        //default for ADS1220: SPI_POL0_PHA1; // ADS1220: Only SPI mode 1 (CPOL = 0, CPHA = 1) is supported.
        //        for SD card: SPI_POL0_PHA0
        spiParams.mode = SPI_MASTER;
        spiParams.bitRate = 500000; //SD-card: 100000, default: 500000; /*!< SPI bit rate in Hz  //max can be 2 MHz.

or:


 ads->spi_trans.cpol = SPICpolIdleLow;
  ads->spi_trans.cpha = SPICphaEdge2;
  ads->spi_trans.dss = SPIDss8bit;
  ads->spi_trans.bitorder = SPIMSBFirst;
  ads->spi_trans.cdiv = SPIDiv128; // f_PCLK / div



 */

void spi_DeInit()
{
	HAL_SPI_DeInit(&hspi3);
}

/******************************************************************************
 *
 * Implementation of the generic SPI functions
 *
 *****************************************************************************/

static inline void SpiSlaveUnselect(uint8_t slave)
{
	HAL_GPIO_WritePin(LOADCELL_CS_N_GPIO_Port, LOADCELL_CS_N_Pin, 1);
}

static inline void SpiSlaveSelect(uint8_t slave)
{
	HAL_GPIO_WritePin(LOADCELL_CS_N_GPIO_Port, LOADCELL_CS_N_Pin, 0);
}

void spi_slave_select(uint8_t slave)
{
//  Semaphore_pend((Semaphore_Handle)semSPI, 10000);
  SpiSlaveSelect(slave);
}

void spi_slave_unselect(uint8_t slave)
{
  SpiSlaveUnselect(slave);
//  Semaphore_reset((Semaphore_Handle)semSPI, 1);
}

void spi_init_slaves(void)
{
	SpiSlaveUnselect(0);
//	  SpiSlaveUnselect(nbox_sd_spi_cs_n);
}


int spi_submit(struct spi_transaction *t)
{
	HAL_StatusTypeDef transferOK;
	unsigned int transfer_count = t->output_length;
	if(transfer_count == 0)
		transfer_count = t->input_length;

	SpiSlaveSelect(0);

	transferOK = HAL_SPI_TransmitReceive(&hspi3, t->output_buf, t->input_buf, transfer_count, 100);

	SpiSlaveUnselect(0);


	if (transferOK != HAL_OK) {
		t->status = SPITransFailed;

		return 0;/* Error in SPI transfer or transfer is already in progress */
	}

	t->status = SPITransSuccess;
	return 1;
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
