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
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define RS485_VCC_Pin GPIO_PIN_13
#define RS485_VCC_GPIO_Port GPIOC
#define RS485_SWH_Pin GPIO_PIN_14
#define RS485_SWH_GPIO_Port GPIOC
#define BEEP_Pin GPIO_PIN_15
#define BEEP_GPIO_Port GPIOC
#define LED1_Pin GPIO_PIN_0
#define LED1_GPIO_Port GPIOC
#define LED2_Pin GPIO_PIN_1
#define LED2_GPIO_Port GPIOC
#define LED3_Pin GPIO_PIN_2
#define LED3_GPIO_Port GPIOC
#define LED4_Pin GPIO_PIN_3
#define LED4_GPIO_Port GPIOC
#define ADC_LIGHT_Pin GPIO_PIN_0
#define ADC_LIGHT_GPIO_Port GPIOA
#define ADC_PWR_Pin GPIO_PIN_1
#define ADC_PWR_GPIO_Port GPIOA
#define SPI1_CS_Pin GPIO_PIN_4
#define SPI1_CS_GPIO_Port GPIOA
#define LCD_RES_Pin GPIO_PIN_6
#define LCD_RES_GPIO_Port GPIOA
#define KEY1_Pin GPIO_PIN_4
#define KEY1_GPIO_Port GPIOC
#define KEY2_Pin GPIO_PIN_5
#define KEY2_GPIO_Port GPIOC
#define KEY3_Pin GPIO_PIN_0
#define KEY3_GPIO_Port GPIOB
#define MOD_VCC_Pin GPIO_PIN_1
#define MOD_VCC_GPIO_Port GPIOB
#define MOD_PWRKEY_Pin GPIO_PIN_12
#define MOD_PWRKEY_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
