/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : bsp.h
  * @brief          : Header for bsp.c file.
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
#ifndef __BSP_H__
#define __BSP_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include "main.h"
#include "et_os.h"
#include "tim.h"

//#include "delay.h"
//#include "fm24cl04.h"
//#include "cmd_queue.h"
//#include "usart.h"
//#include "adc.h"
//#include "spi.h"
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

/* USER CODE BEGIN EFP */
void bsp_config(void);
void SendChar(uint8_t t);


/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define CACHE 5 //二位数组缓存的数量
#define RX_NUM 24//接收数据最多24个
#define rx_start_tim 28 	//启动接收信号
//需要看发送端发送时间  单位是100us
#define rx_0_up 	 	205  	//接收判断为0的上限  
#define rx_0_down  	195		//接收判断为0的下限

#define rx_1_up   	225 		//接收判断为1的上限
#define rx_1_down 	215 		//接收判断为1的下限


extern uint8_t rx_byte[CACHE][32];//接收每个字节的时间计数；


#ifdef __cplusplus
}
#endif

#endif /* __BSP_H__ */

