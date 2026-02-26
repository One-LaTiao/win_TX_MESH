/**
 * @file 
 * @author yh 
 * @brief
 * @version 0.0
 * @date 2025_06_28
 *
 * @copyright 
 */
#include "config.h"


static tmr_t tmr_start_task02_key;
static void task02_key(int timer_id, void *data);


//将任务加入队列
void start_task02_key(void)
{
	start_rpt_tmr(&tmr_start_task02_key,task02_key, MS_TO_TICKS(10));
}

//任务实现
/*按键开关控制开关——点动控制*/
static void task02_key(int timer_id,void *data)
{


}
	
