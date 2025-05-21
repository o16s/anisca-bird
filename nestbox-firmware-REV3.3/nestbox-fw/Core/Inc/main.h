/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#define NAND_FLASH_USE_LEGACY_FTL	// allows to use FTL with continuous logs written to flash, instead of accessing specific sectors.

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
#define DEBUG_LED_OFF	0
#define DEBUG_LED_ON	0 // TODO-PROD: keep LED off in production code.
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define PWR_LOAD_CELL_LDO_EN_GPIO_Port 	PWR_GPIO_A
#define PWR_BAT_TEST_EN_GPIO_Port 		PWR_GPIO_A
#warning "These constants are not updated. Check with port names below"
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void system_restore_clock();
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LOADCELL_DRDY_N_Pin GPIO_PIN_13
#define LOADCELL_DRDY_N_GPIO_Port GPIOC
#define LOADCELL_DRDY_N_EXTI_IRQn EXTI15_10_IRQn
#define USER_GPIO_Pin GPIO_PIN_0
#define USER_GPIO_GPIO_Port GPIOA
#define USER_GPIO_EXTI_IRQn EXTI0_IRQn
#define FLASH_POWER_ON_N_Pin GPIO_PIN_1
#define FLASH_POWER_ON_N_GPIO_Port GPIOA
#define VBAT_ADC9_Pin GPIO_PIN_4
#define VBAT_ADC9_GPIO_Port GPIOA
#define BAT_TEST_EN_Pin GPIO_PIN_5
#define BAT_TEST_EN_GPIO_Port GPIOA
#define DEBUG_LED_RTC_CALIB_Pin GPIO_PIN_2
#define DEBUG_LED_RTC_CALIB_GPIO_Port GPIOB
#define WIFI_EN_Pin GPIO_PIN_12
#define WIFI_EN_GPIO_Port GPIOB
#define ADC_CLK_EN_Pin GPIO_PIN_13
#define ADC_CLK_EN_GPIO_Port GPIOB
#define RFID_SHDN_Pin GPIO_PIN_14
#define RFID_SHDN_GPIO_Port GPIOB
#define RFID_SUPPLY_EN_Pin GPIO_PIN_15
#define RFID_SUPPLY_EN_GPIO_Port GPIOB
#define LOAD_CELL_LDO_EN_Pin GPIO_PIN_8
#define LOAD_CELL_LDO_EN_GPIO_Port GPIOA
#define LOADCELL_CS_N_Pin GPIO_PIN_15
#define LOADCELL_CS_N_GPIO_Port GPIOA
#define EXC_B_N_Pin GPIO_PIN_6
#define EXC_B_N_GPIO_Port GPIOB
#define EXC_B_P_Pin GPIO_PIN_7
#define EXC_B_P_GPIO_Port GPIOB
#define EXC_A_P_Pin GPIO_PIN_8
#define EXC_A_P_GPIO_Port GPIOB
#define EXC_A_N_Pin GPIO_PIN_9
#define EXC_A_N_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
