/**
  ******************************************************************************
  * @file    spi.h
  * @brief   This file contains all the function prototypes for
  *          the spi.c file
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
#ifndef __SPI_H__
#define __SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern SPI_HandleTypeDef hspi3;

/* USER CODE BEGIN Private defines */


struct spi_transaction;


/** SPI Callback function.
 * If not NULL (or 0), call function (with transaction as parameter)
 * before or after transaction, e.g to allow execution of hardware specific actions
 */
typedef void (*SPICallback)(struct spi_transaction *trans);



/** SPI transaction status.
 */
enum SPITransactionStatus {
  SPITransPending,
  SPITransRunning,
  SPITransSuccess,
  SPITransFailed,
  SPITransDone
};

/** SPI transaction structure.
 * - Use this structure to store a request of SPI transaction
 *   and submit it using #spi_submit function
 * - The input/output buffers needs to be created separately
 * - Take care of pointing input_buf/ouput_buf correctly
 * - input_length and output_length can be different, the larger number
 *   of the two specifies the total number of exchanged words,
 * - if input_length is larger than output length,
 *   0 is sent for the remaining words
 */
struct spi_transaction {
   uint8_t *input_buf;  ///< pointer to receive buffer for DMA
   uint8_t *output_buf; ///< pointer to transmit buffer for DMA
  uint16_t input_length;        ///< number of data words to read
  uint16_t output_length;       ///< number of data words to write
  SPICallback before_cb;        ///< NULL or function called before the transaction
  SPICallback after_cb;         ///< NULL or function called after the transaction
  volatile enum SPITransactionStatus status;
};

/* USER CODE END Private defines */

void MX_SPI3_Init(void);

/* USER CODE BEGIN Prototypes */

void spi_DeInit();

int spi_submit(struct spi_transaction *t);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __SPI_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
