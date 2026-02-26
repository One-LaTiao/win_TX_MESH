#include "bsp.h"
#include "cmd_queue.h"


uint8_t uart_data = 0;
//uint8_t uart2_data = 0;



/**************************串口功能****************************************************************************** */
/*串口中断接收回调*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart == &huart1)
	{
		queue_push(uart_data);
		HAL_UART_Receive_DMA(huart,&uart_data,1);
	}

}

/*屏幕使用*/
void SendChar(uint8_t t)
{
		HAL_UART_Transmit_DMA(&huart1,&t,1);
}



//软件使用
//void SendChar_ui(uint8_t *t,uint8_t len)
//{
//	for(uint8_t i = 0;i<len;i++)
//	{
//		HAL_UART_Transmit(&huart1,&t[i],1,10);
//	}
//}




/**************************ADC功能****************************************************************************** */

//void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
//{
//	if (hadc->Instance == ADC1)  // 根据实际使用的DMA通道修改
//	{

//	}
//}

/**************************SPIC功能****************************************************************************** */


//void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
//{
//	if (hspi == &hspi2)
//	{
//		HAL_GPIO_WritePin(spi2_CS_GPIO_Port,spi2_CS_Pin,GPIO_PIN_SET);
//		for(uint8_t i = 0; i < 13; i++)
//		{
//			printf("temp[%d] = %d \r\n",i,rxdata[i]);
//		}
//	}
//}


//看门狗
//static void tmr_iwdg_callback(int timer_id, void *data)
//{
//    HAL_IWDG_Refresh(&hiwdg);
//    start_tmr(&tmr_iwdg_handle, tmr_iwdg_callback, MS_TO_TICKS(400));
//}
/*******************外部中断IO*************************************************************************************************/
//火蝠模块最快100us，最慢100ms；
//发送端发送时间的时间比接收端少2ms；例如发送2ms秒，接收4ms；


//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
//{


//	
//}



/*配置初始化*/
void bsp_config(void)
{
	HAL_UART_Receive_DMA(&huart1,&uart_data,1);

}

