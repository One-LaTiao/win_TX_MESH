/**
 *******************************************************************************
 * @file    et_timer.h
 * @author  yeelight
 * @version
 * @date    2022-11-01
 * @brief
 *          add MS_TO_TICKS, US_TO_TICKS
 *
 *******************************************************************************
 * @attention
 *
 *
 *******************************************************************************
 */

/*******************************************************************************
 ********************* define to prevent recursive inclusion *******************
 ******************************************************************************/
#ifndef _ET_TIMER_H_
#define _ET_TIMER_H_

/*******************************************************************************
 ********************************* include files *******************************
 ******************************************************************************/
#include <stdint.h>
#include "klist.h"
#include "et_os.h"
#include "bsp.h"


/*******************************************************************************
 ************************ exported macros and struct types *********************
 ******************************************************************************/
//#define TIMER_F            500000ULL
//#define TICKS_PER_S        1000000ULL // 1M
#define TIMER_TICK_RATE_HZ 1000000ULL // 1M

//定时器精度为 1us  --> 1M
#define MS_PER_TICK    1   // 0.001 ms = 1 microsec   unit: microsecond (us)

#define MS_TO_TICKS(time_in_ms) (((tick_type_t)(time_in_ms) * TIMER_TICK_RATE_HZ) / (tick_type_t) 1000)
#define US_TO_TICKS(time_in_us) (((tick_type_t)(time_in_us) * TIMER_TICK_RATE_HZ) / (tick_type_t) 1000000)

typedef uint64_t tick_type_t;
typedef uint64_t timer_ms_t;
typedef void (*timer_func_t)(int timer_id, void *data);


typedef uint64_t microsec_t;
typedef uint32_t millisec_t;
microsec_t get_sys_time_micro(void);
millisec_t get_sys_time_ms(void);


typedef struct {
    struct list_head self;		/*链表*/
    int              timer_id;/*定时器编号*/
    unsigned         val; 		/*等待周期*/          /* how many ticks? */
    timer_ms_t       expire;	/*是否过期*/
		int              repeat;	/*是否重复*/
    timer_func_t     fn;			/*执行函数*/
    void            *data;		/*函数传参*/
} tmr_t;

#define start_tmr(tmr, func, to)              \
    do {                                      \
        (tmr)->val = to;                      \
        (tmr)->timer_id = 0;                  \
        (tmr)->data = NULL;                   \
        (tmr)->fn = func;                     \
        (tmr)->repeat = 0;                    \
        add_timer((tmr));                     \
    } while (0)

#define restart_tmr(tmr, to)                  \
    do {                                      \
        (tmr)->val = to;                      \
        (tmr)->timer_id = 0;                  \
        timer_re_add((tmr));                  \
    } while (0)

#define start_tmr_with_data(tmr, func, to, d) \
    do {                                      \
        (tmr)->val = to;                      \
        (tmr)->timer_id = 0;                  \
        (tmr)->data = d;                      \
        (tmr)->fn = func;                     \
        (tmr)->repeat = 0;                    \
        add_timer((tmr));                     \
    } while (0)
		
		
		/*添加重复定时器*/
#define start_rpt_tmr(tmr, func, to)          \
    do {                                      \
        (tmr)->val = to;                      \
        (tmr)->timer_id = 0;                  \
        (tmr)->data = NULL;                   \
        (tmr)->fn = func;                     \
        (tmr)->repeat = 1;                    \
        add_timer((tmr));                     \
    } while (0)

#define start_rpt_tmr_with_data(tmr, func, to, d)    \
    do {                                             \
        (tmr)->val = to;                             \
        (tmr)->timer_id = 0;                         \
        (tmr)->data = d;                             \
        (tmr)->fn = func;                            \
        (tmr)->repeat = 1;                           \
        add_timer((tmr));                            \
    } while (0)

#define start_tmr_with_id(tmr, func, to, tid)        \
    do {                                             \
        (tmr)->val = to;                             \
        (tmr)->timer_id = tid;                       \
        (tmr)->data = NULL;                          \
        (tmr)->fn = func;                            \
        (tmr)->repeat = 0;                           \
        add_timer((tmr));                            \
    } while (0)

#define start_tmr_with_data_id( tmr, func, to, d, tid)  \
    do {                                                \
        (tmr)->val = to;                                \
        (tmr)->timer_id = tid;                          \
        (tmr)->data = d;                                \
        (tmr)->fn = func;                               \
        (tmr)->repeat = 0;                              \
        add_timer((tmr));                               \
    } while (0)

#define tmr_shorten(tmr, func, cmp, to)                 \
    if (time2expire(tmr) > (cmp) * MS_PER_TICK) {       \
        start_tmr(tmr, func, to);                       \
    }

/*******************************************************************************
 ******************************* exported functions ****************************
 ******************************************************************************/
extern uint32_t get_sys_ms_now(void);
extern void timer_init(void);
extern void add_timer(tmr_t *timer);
extern void del_timer(tmr_t *timer);
extern void proc_timer(void);
extern int  timer_is_running(tmr_t *timer);
extern timer_ms_t timer_expire(tmr_t *timer);
extern int time2expire(void);
extern int get_next_timeout(timer_ms_t *tick);

extern volatile microsec_t m433_timer ;//信号计时长度
extern volatile uint8_t tx_sta;  //0表示空闲状态
extern volatile uint32_t tx_data;

#endif
/********************************* end of file ********************************/

