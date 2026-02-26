/**
 *******************************************************************************
 * @file    et_timer.c
 * @author  yeelight
 * @version 1.0.0
 * @date    2022-11-01
 * @brief   V1 2022-11-01
 *          create
 *
 *******************************************************************************
 * @attention
 *
 *
 *******************************************************************************
 */

/*******************************************************************************
 * include files
 ******************************************************************************/
#include "et_timer.h"

/*******************************************************************************
 * private define macro and struct types
 ******************************************************************************/
/* t1 is earlier than t2 */
#define time_before(t1, t2)      ((int)((t1) - (t2)) <= 0)
#define timer_before(tmr1, tmr2) (time_before(tmr1->expire, tmr2->expire))

/*计数器链表*/
typedef struct tmr_cb {
    struct list_head  tmr_list;/*链表*/
    volatile timer_ms_t  sys_msec;/*计数器*/
} tmr_cb_t;

static tmr_cb_t thdl;

/*******************************************************************************
 *******************************************************************************
 * private application code, functions definitions
 *******************************************************************************
 ******************************************************************************/
/**
 * Update the current time.更新当前时间。
 * Because all timer's expiring time is relative to current time, so we must
 * update current time after each time-consuming operations.
 */
/*向定时器获取时间*/
static void time_update(void)
{
    thdl.sys_msec = get_sys_time_micro();

    return;
}

void timer_init(void)
{
    INIT_LIST_HEAD(&(thdl.tmr_list));/*创建链表头*/

    time_update();/*更新表时间*/
}
/*删除定时器*/
void del_timer(tmr_t *timer)
{
    if (timer->self.next != NULL && timer->self.prev != NULL) {
        list_del(&(timer->self));
    }
}

/**
 * Place the timer into timer queue.
 *将定时器放入定时器队列。
 */
void add_timer(tmr_t *timer)
{
    tmr_t *tmr;

    del_timer(timer); // try del it before add timer

    timer_ms_t now_ms = thdl.sys_msec;

    if (time_before(now_ms, timer->expire + MS_PER_TICK) && time_before(timer->expire, now_ms)) {
        /* assume timer was restarted */
        timer->expire = timer->expire + timer->val * MS_PER_TICK;
    } else {
        timer->expire = now_ms + timer->val * MS_PER_TICK;
    }

    INIT_LIST_HEAD(&(timer->self));

    list_for_each_entry(tmr, &(thdl.tmr_list), self) {
        if (timer_before(timer, tmr)) {
            break;
        }
    }
    //OS_CPU_SR cpu_sr;

    //enter_critical();
    list_add_tail(&(timer->self), &(tmr->self));
    //exit_critical();

}

/**
 * Reset timer based on last expiration time
 * Place the timer into timer queue.
 */
void timer_re_add(tmr_t *timer)
{
    tmr_t *tmr;

    timer->expire += timer->val * MS_PER_TICK;
    INIT_LIST_HEAD(&(timer->self));

    list_for_each_entry(tmr, &(thdl.tmr_list), self) {
        if (timer_before(timer, tmr)) {
            break;
        }
    }

    list_add_tail(&(timer->self), &(tmr->self));
}

/*
 * timer is running, return 1, other return 0
 * 定时器正在运行，返回 1，否则返回 0。
 */
int timer_is_running(tmr_t *timer)
{
    if (timer->self.next != NULL && timer->self.prev != NULL) {
        return 1;
    }
    return 0;
}

/*
 * return the remain time. unit: ms
 */
timer_ms_t timer_expire(tmr_t *timer)
{
    tmr_cb_t *tcb = (tmr_cb_t *)&thdl;

    if (timer_is_running(timer)) {
        if (time_before(timer->expire, tcb->sys_msec)) {
            return 0;
        } else {
            return (timer->expire - tcb->sys_msec);
        }
    }

    return 0;
}

/**
 * Do callbacks for all the expired timer, restart the timer
 * if it's repeatitive.
 *对所有已到期的定时器执行回调，如果它是重复的，则重新启动定时器。
 */
void proc_timer(void)
{
	tmr_cb_t *tcb = (tmr_cb_t *)&thdl;
	tmr_t *tmr;

	time_update();

	for (;;) 
	{
				if (list_empty(&(tcb->tmr_list))) 
				{/*检测列是否为空*/
						break;
				}
				/*获取列表的第一个元素*/
				tmr = list_first_entry(&(tcb->tmr_list), tmr_t, self);

				if (time_before(tmr->expire, tcb->sys_msec)) 
				{/*判断是否过期*/
						del_timer(tmr);/*删除定时器*/
						if (tmr->repeat)/*判断是否重复*/ 
						{
								add_timer(tmr);/*重新加入到队列中*/
						}
						tmr->fn(tmr->timer_id, tmr->data);/*执行任务*/
				} else {
						break;
				}
		}
}

/**
 * Find out how much time can we sleep before we need to
 * wake up to handle the timer.
 */
int get_next_timeout(timer_ms_t *tick)
{
    tmr_cb_t *tcb = (tmr_cb_t *)&thdl;
    tmr_t *tmr;

    if (list_empty(&(tcb->tmr_list))) {
        return -1;
    }

    tmr = list_first_entry(&(tcb->tmr_list), tmr_t, self);

    if (time_before(tmr->expire, tcb->sys_msec)) {
        *tick = 0;
    } else {
        *tick = tmr->expire - tcb->sys_msec;
    }

    return 0;
}

int time2expire(void)
{
    tmr_cb_t *tcb = (tmr_cb_t *)&thdl;
    if (list_empty(&(tcb->tmr_list))) {
        return  24 * 3600 * 1000ull; // 18h is insanely long
    }
    tmr_t *tmr = list_first_entry(&(tcb->tmr_list), tmr_t, self);

    time_update();

    if (!tmr || !timer_is_running(tmr))
        return  24 * 3600 * 1000ull; // 18h is insanely long

    return tmr->expire - tcb->sys_msec;
}





/* PWM输出引脚 */
//#define PWM_PORT  led_GPIO_Port
//#define PWM_PIN   led_Pin
//volatile uint8_t current_pwm = 3;/*数值越大，电流越大*/

 /**
  * @brief PWM控制输出
  * @note  
  * @retval none
	*/
//void pos_pwm_callback(void)
//{
//	static volatile uint8_t   current_timer=0;
//	HAL_GPIO_WritePin(vref_out_GPIO_Port,vref_out_Pin,(current_timer < current_pwm) ? GPIO_PIN_SET : GPIO_PIN_RESET);/*电流调节*/
//	if(current_timer++ > 150) current_timer = 0;
//}


#define BIT_NUM 24
#define TX_0 10
#define TX_1 25

#define START_SIGNAL 109

volatile microsec_t m433_timer = 0;//信号计时长度
volatile uint8_t tx_sta = 0;  	//0表示空闲状态,2标识接下来发高电平时间
volatile uint8_t bit_count = 0;  	//发送数量计数
volatile uint32_t tx_data = 0;

void m433_tx(void)
{
	if(tx_sta < 1){ /*当发送标志位为0时，停止发送*/
		return ; 
	}
	
	if(bit_count > BIT_NUM){
		tx_sta =0;/*关闭发送开关*/
		bit_count = 0; /*发送数量清零*/
		HAL_GPIO_WritePin(dat_433m_GPIO_Port,dat_433m_Pin,GPIO_PIN_SET);
		m433_timer = 0;
		return ;
	}
	
	if(tx_sta == 1){/*启动信号，低电平时间11ms，*/
		HAL_GPIO_WritePin(dat_433m_GPIO_Port,dat_433m_Pin,GPIO_PIN_RESET);
		if(m433_timer++ > START_SIGNAL){
			m433_timer = 0;
			tx_sta = 2;
			return ;
		}
	}
	
	
	if(tx_sta == 2){/*当开启发送时，先发送低电平*/
		HAL_GPIO_WritePin(dat_433m_GPIO_Port,dat_433m_Pin,GPIO_PIN_SET);
		if(((tx_data >> bit_count) & 0x00000001) == 0x00){/*如果发送的数据是0，则根据0的发送时间来计算*/
			if(m433_timer++ > TX_0){
				bit_count++;
				m433_timer = 0;
				tx_sta = 3;
			}
		}else{/*如果发送的数据是1，则根据1的发送时间来计算*/
			if(m433_timer++ > TX_1){
				bit_count++;
				m433_timer = 0;
				tx_sta = 3;
			}
		}
	}else if(tx_sta == 3){/*当低电平发送完成，再发送高电平电平*/
		HAL_GPIO_WritePin(dat_433m_GPIO_Port,dat_433m_Pin,GPIO_PIN_RESET);
		if(((tx_data >> bit_count) & 0x00000001) == 0x00){/*如果发送的数据是0，则根据0的发送时间来计算*/
			if(m433_timer++ > TX_0){
				bit_count++;
				m433_timer = 0;
				tx_sta = 2;
			}
		}else{
			if(m433_timer++ > TX_1){/*如果发送的数据是1，则根据1的发送时间来计算*/
				bit_count++;
				m433_timer = 0;
				tx_sta = 2;
			}
		}
	}
}



static volatile microsec_t timer_counter = 0;

/*定时器中断回调函数*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &htim3){
        timer_counter ++;
    }
		if(htim == &htim1)
		{
			m433_tx();
//		 HAL_GPIO_TogglePin(dat_433m_GPIO_Port,dat_433m_Pin);
		}
}
//Get time value
microsec_t get_sys_time_micro(void)
{
// We need to record the old time value,
// because it is not guaranteed that the TIM2 event interrupt
// will not be triggered during the call of this function.
  static uint64_t old_time_val;
  uint64_t new_time_val;
	
  new_time_val = (timer_counter * 10000) + TIM3->CNT;
  //When the calculated time value is less than the previous time value,
	//it indicates that a TIM2 event interrupt occurred during the function calculation,
	//and the time value should be corrected.
  if (new_time_val < old_time_val) new_time_val += 10000;
  old_time_val = new_time_val;
  return new_time_val;
}

millisec_t get_sys_time_ms(void)
{
	 return (millisec_t)(get_sys_time_micro()/1000);
}



/********************************* end of file ********************************/
