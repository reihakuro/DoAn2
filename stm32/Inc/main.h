/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2026 STMicroelectronics.
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
#define MOTOR_IN1_Pin GPIO_PIN_1
#define MOTOR_IN1_GPIO_Port GPIOA
#define MOTOR_IN2_Pin GPIO_PIN_2
#define MOTOR_IN2_GPIO_Port GPIOA
#define MOTOR_IN3_Pin GPIO_PIN_4
#define MOTOR_IN3_GPIO_Port GPIOA
#define MOTOR_IN4_Pin GPIO_PIN_5
#define MOTOR_IN4_GPIO_Port GPIOA
#define PWR_HOLD_Pin GPIO_PIN_0
#define PWR_HOLD_GPIO_Port GPIOB
#define PWR_BTN_Pin GPIO_PIN_1
#define PWR_BTN_GPIO_Port GPIOB
#define PI_HALT_Pin GPIO_PIN_10
#define PI_HALT_GPIO_Port GPIOB
#define L_TRIG_Pin GPIO_PIN_12
#define L_TRIG_GPIO_Port GPIOB
#define L_ECHO_Pin GPIO_PIN_13
#define L_ECHO_GPIO_Port GPIOB
#define M_TRIG_Pin GPIO_PIN_14
#define M_TRIG_GPIO_Port GPIOB
#define M_ECHO_Pin GPIO_PIN_15
#define M_ECHO_GPIO_Port GPIOB
#define BZ_Pin GPIO_PIN_5
#define BZ_GPIO_Port GPIOB
#define EN_EXT_L_Pin GPIO_PIN_6
#define EN_EXT_L_GPIO_Port GPIOB
#define EN_EXT_L_EXTI_IRQn EXTI9_5_IRQn
#define EN_EXT_R_Pin GPIO_PIN_7
#define EN_EXT_R_GPIO_Port GPIOB
#define EN_EXT_R_EXTI_IRQn EXTI9_5_IRQn
#define R_TRIG_Pin GPIO_PIN_8
#define R_TRIG_GPIO_Port GPIOB
#define R_ECHO_Pin GPIO_PIN_9
#define R_ECHO_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
