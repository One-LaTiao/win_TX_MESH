/**************************************************************************************************
* Filename:             config.h
* Revised:
* Revision:
* Description:
**************************************************************************************************/
#ifndef _CONFIG_H
#define _CONFIG_H

#ifdef __cplusplus

extern "C"
{

#endif

#include <stdio.h>
#include "bsp.h"


/*
 * 调试信息
 */
#define USING_DEBUG
#ifdef USING_DEBUG
#define log_level  0
#define log_printf(level,...)\
    {\
        if (level <= log_level)\
        {\
            printf(__VA_ARGS__);\
        }\
    }
#define log_err(...)  log_printf(1, __VA_ARGS__)
#define log_warn(...) log_printf(2, __VA_ARGS__)
#define log_info(...) log_printf(3, __VA_ARGS__)
#define log_dbg(...)  log_printf(4, __VA_ARGS__)
#define log_hex(x,len)    do { int i = 0;uint8_t *p=x;for(i=0;i<len;i++) log_info("%02X",p[i]); log_info("\r\n");} while (0)
# define assert(p) do { \
        if (!(p)) { \
            log_err("BUG at assert(%s)\n", #p); \
        }       \
    } while (0)
#else
#define log_err(...)
#define log_warn(...)
#define log_info(...)
#define log_dbg(...)
#define log_hex(x,len)
#define assert(p) ((void)0)
#endif

#define FW_NAME            "CRN_motor_control"                    // 固件名
#define FW_VERSION_MAJOR   0                                    // 主版本号
#define FW_VERSION_MINOR   0                                    // 次版本号
#define FW_VERSION_patch   0                                    // 修订版本号

		
/* Exported constants --------------------------------------------------------*/
extern uint8_t addr_value[9];//当前地址

/* Exported functions prototypes ---------------------------------------------*/
void start_task01_data(void);
void start_task02_key(void);
void start_task04_hmi(void);


		
#ifdef __cplusplus
}
#endif

#endif
