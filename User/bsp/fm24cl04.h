/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : fm24cl04.h
  * @brief          : Header for fm24cl04.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024.12.10 STMicroelectronics.
  * All rights reserved.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FM24CL04_H__
#define __FM24CL04_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* USER CODE BEGIN Private defines */

#define FM24CL16_Write_1 0xA0
#define FM24CL16_Read_1	 0xA1
#define FM24CL16_Write_2 0xA2
#define FM24CL16_Read_2	 0xA3
#define FM24CL16_Write_3 0xA4
#define FM24CL16_Read_3	 0xA5


/* Private includes ----------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/


/* Exported constants --------------------------------------------------------*/


/* Exported macro ------------------------------------------------------------*/


/* Exported functions prototypes ---------------------------------------------*/
/*速度写入*/
void temp_write(uint8_t data);
/*速度读出*/
void temp_read(uint8_t *data);
void over_temp_read(uint8_t *data);
void over_temp_write(uint8_t data);



/* Private defines -----------------------------------------------------------*/



#ifdef __cplusplus
}
#endif

#endif /* __FM24CL04_H__ */

