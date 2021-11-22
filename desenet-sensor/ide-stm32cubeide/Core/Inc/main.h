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
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define B1_EXTI_IRQn EXTI15_10_IRQn
#define ACCEL_CS_Pin GPIO_PIN_2
#define ACCEL_CS_GPIO_Port GPIOC
#define ACCEL_IRQ1_Pin GPIO_PIN_3
#define ACCEL_IRQ1_GPIO_Port GPIOC
#define ACCEL_IRQ1_EXTI_IRQn EXTI3_IRQn
#define ACCEL_ADC1_Pin GPIO_PIN_0
#define ACCEL_ADC1_GPIO_Port GPIOA
#define ACCEL_ADC2_Pin GPIO_PIN_1
#define ACCEL_ADC2_GPIO_Port GPIOA
#define STLINK_TX_Pin GPIO_PIN_2
#define STLINK_TX_GPIO_Port GPIOA
#define STLINK_RX_Pin GPIO_PIN_3
#define STLINK_RX_GPIO_Port GPIOA
#define ACCEL_ADC3_Pin GPIO_PIN_4
#define ACCEL_ADC3_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define TRACE_CTS_Pin GPIO_PIN_6
#define TRACE_CTS_GPIO_Port GPIOA
#define TRACE_TX_Pin GPIO_PIN_4
#define TRACE_TX_GPIO_Port GPIOC
#define TRACE_RX_Pin GPIO_PIN_5
#define TRACE_RX_GPIO_Port GPIOC
#define ACCEL_IRQ2_Pin GPIO_PIN_0
#define ACCEL_IRQ2_GPIO_Port GPIOB
#define ACCEL_IRQ2_EXTI_IRQn EXTI0_IRQn
#define TRACE_RTS_Pin GPIO_PIN_1
#define TRACE_RTS_GPIO_Port GPIOB
#define DISP_SPI2_SCK_Pin GPIO_PIN_13
#define DISP_SPI2_SCK_GPIO_Port GPIOB
#define DISP_SPI2_MISO_Pin GPIO_PIN_14
#define DISP_SPI2_MISO_GPIO_Port GPIOB
#define DISP_SPI2_MOSI_Pin GPIO_PIN_15
#define DISP_SPI2_MOSI_GPIO_Port GPIOB
#define DISP_RESET_Pin GPIO_PIN_7
#define DISP_RESET_GPIO_Port GPIOC
#define NAVS_RIGHT_Pin GPIO_PIN_8
#define NAVS_RIGHT_GPIO_Port GPIOC
#define NAVS_RIGHT_EXTI_IRQn EXTI9_5_IRQn
#define NAVS_UP_Pin GPIO_PIN_9
#define NAVS_UP_GPIO_Port GPIOC
#define NAVS_UP_EXTI_IRQn EXTI9_5_IRQn
#define DISP_DATA_Pin GPIO_PIN_9
#define DISP_DATA_GPIO_Port GPIOA
#define NAVS_DOWN_Pin GPIO_PIN_10
#define NAVS_DOWN_GPIO_Port GPIOA
#define NAVS_DOWN_EXTI_IRQn EXTI15_10_IRQn
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define NRF_CS_Pin GPIO_PIN_15
#define NRF_CS_GPIO_Port GPIOA
#define NRF_CLK_Pin GPIO_PIN_10
#define NRF_CLK_GPIO_Port GPIOC
#define NRF_MISO_Pin GPIO_PIN_11
#define NRF_MISO_GPIO_Port GPIOC
#define NRF_MOSI_Pin GPIO_PIN_12
#define NRF_MOSI_GPIO_Port GPIOC
#define NRF_CE_Pin GPIO_PIN_2
#define NRF_CE_GPIO_Port GPIOD
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define NAVS_LEFT_Pin GPIO_PIN_4
#define NAVS_LEFT_GPIO_Port GPIOB
#define NAVS_LEFT_EXTI_IRQn EXTI4_IRQn
#define NAVS_CENTER_Pin GPIO_PIN_5
#define NAVS_CENTER_GPIO_Port GPIOB
#define NAVS_CENTER_EXTI_IRQn EXTI9_5_IRQn
#define DISP_BUSY_Pin GPIO_PIN_6
#define DISP_BUSY_GPIO_Port GPIOB
#define NRF_IRQ_Pin GPIO_PIN_7
#define NRF_IRQ_GPIO_Port GPIOB
#define NRF_IRQ_EXTI_IRQn EXTI9_5_IRQn
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
