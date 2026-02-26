#include "delay.h"



uint32_t YH_Delay[50] = {0};//yan shi shi jian 
uint32_t YH_StartWait[50] = {0};//yan shi kai shi shi jian ji lu 
uint32_t YH_Run_Time[50]={0};//yan shi shi shi shi jian



	/* yanshihanshu
uint8_t num,yan,shi bian hao 0-19 ge;
uint32_t Delay yan shi shi chang;
*/
bool TRD(uint8_t num,uint32_t Delay)
{
	
	uint32_t System_time = HAL_GetTick();
	
	/* if the delay is equal to zero,return false  */
	if(Delay == 0)
		return false;
	/* if the YH_Delay[num] == YH_Run_Time[num],return true  */
	if(YH_Run_Time[num] >= Delay)
		return true;
	/* Calculate delay time */
		if(YH_Run_Time[num] > 0)
	{
		/* YH_Run_Time[num] > 0 Calculate delay time  */
		if(System_time >= YH_StartWait[num])
		{
				/* Calculate Runing Time  */
			YH_Run_Time[num] = System_time - YH_StartWait[num];
			return false;
		}
		else
		{
			/* ç³»ç»Ÿæ—¶é—´< å¼?å§‹æ—¶é—´ï¼Œè¯´æ˜Žç³»ç»Ÿæ—¶é—´æº¢å‡ºï¼Œé‡æ–°èµ‹å€?*/
			YH_StartWait[num] = System_time;
		}
		return false;
	}
	/* if YH_Run_Time[num] ==0 Record Start Time*/
	if(YH_Run_Time[num] == 0)
	{
		YH_StartWait[num] = System_time;//
		YH_Run_Time[num] = 1;//
	}
		return false;
}


/* Rest one delay timer*/
void Rest_TRD(uint8_t num)
{
	YH_StartWait[num]= 0;//
  YH_Run_Time[num]= 0;//
}

//void delay_ms(void)
//{
//	if(TRD(1,500))
//	{//500msÒ»´ÎÂö³å
//		Rest_TRD(1);
//	}
//	if(TRD(2,200))
//	{//200msÒ»´ÎÂö³å
//		Rest_TRD(2);
//	}
//	if(TRD(3,100))
//	{//100msÒ»´ÎÂö³å
//		Rest_TRD(3);
//	}
//}

