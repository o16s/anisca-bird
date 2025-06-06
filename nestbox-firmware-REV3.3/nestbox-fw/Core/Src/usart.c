/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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
#include "usart.h"

/* USER CODE BEGIN 0 */
#include <string.h>
/* USER CODE END 0 */

UART_HandleTypeDef huart3;

/* USART3 init function */

void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspInit 0 */

  /* USER CODE END USART3_MspInit 0 */
    /* USART3 clock enable */
    __HAL_RCC_USART3_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**USART3 GPIO Configuration
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* USART3 interrupt Init */
    HAL_NVIC_SetPriority(USART3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART3_IRQn);
  /* USER CODE BEGIN USART3_MspInit 1 */

  /* USER CODE END USART3_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspDeInit 0 */

  /* USER CODE END USART3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART3_CLK_DISABLE();

    /**USART3 GPIO Configuration
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10|GPIO_PIN_11);

    /* USART3 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART3_IRQn);
  /* USER CODE BEGIN USART3_MspDeInit 1 */

  /* USER CODE END USART3_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

#define NEWLINE_CHARACTER	'\n'


void uart_DeInit()
{
	HAL_UART_DeInit(&huart3);
}

#define IO_UART_TX_TIMEOUT 500
//#define IO_UART_RX_TIMEOUT 100

HAL_StatusTypeDef io_uart_rx(uint8_t * data, uint16_t len, uint32_t timeout){
	memset(data, 0, len);

	return HAL_UART_Receive(&huart3, data, len, timeout);

//	uint8_t res = HAL_UART_Receive_DMA(uart_handle, data, len);
//	io_delay(IO_UART_RX_TIMEOUT);
//	HAL_UART_DMAStop(uart_handle);

}


uint8_t usart_rx_buffer[USART_RX_BUFFER_LEN];
static unsigned int usart_rx_index = 0;
static int line_end_reached = 0;

int io_uart_rx_line(uint8_t** buffer, uint32_t timeout){

	uint32_t startime = HAL_GetTick();
	memset(usart_rx_buffer, 0, USART_RX_BUFFER_LEN);

	usart_rx_index = 0;
	line_end_reached = 0;

	HAL_UART_Receive_IT(&huart3, usart_rx_buffer, 1);

	while((line_end_reached == 0) && (usart_rx_index<USART_RX_BUFFER_LEN))
	{
		if(HAL_GetTick() - startime > timeout)
			break;
	}

	*buffer = usart_rx_buffer;
	return usart_rx_index;
}

HAL_StatusTypeDef io_uart_tx(uint8_t * data, uint16_t len){
	return HAL_UART_Transmit(&huart3, data, len, IO_UART_TX_TIMEOUT);

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == &huart3)
	{
		if(usart_rx_buffer[usart_rx_index] == NEWLINE_CHARACTER)
		{
			usart_rx_index++;
			line_end_reached = 1;
		}
		else
		{
			usart_rx_index++;
			if(usart_rx_index<USART_RX_BUFFER_LEN)
				HAL_UART_Receive_IT(&huart3, &usart_rx_buffer[usart_rx_index], 1);
		}
	}
}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
