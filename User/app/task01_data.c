#include "config.h"

/*2025_06_28*/

/*协议收发任务*/
static tmr_t tmr_start_task01_data;
static void task01_rxdata(int timer_id, void *data);/*协议处理*/



//添加任务队列中
void start_task01_data(void)
{
	start_rpt_tmr(&tmr_start_task01_data, task01_rxdata, MS_TO_TICKS(5));
}


/*协议处理*/
static void task01_rxdata(int timer_id, void *data)
{	


	
	
}
































