/**
 *******************************************************************************
 * @file    et_os.h
 * @author  yeelight
 * @version
 * @date    2022-11-11
 * @brief   操作系统层的头文件定义，包括请求类型、结构和函数的声明
 *
 *******************************************************************************
 * @attention
 * 注意事项
 *
 *******************************************************************************
 */

/*******************************************************************************
 ********************* define to prevent recursive inclusion *******************
 ******************************************************************************/
#ifndef _ET_OS_H_ // 防止头文件重复包含
#define _ET_OS_H_

/*******************************************************************************
 ********************************* include files *******************************
 ******************************************************************************/
#include "klist.h"     // 链表实现的头文件
#include "stdint.h"    // 标准整数类型定义
#include "config.h"    // 配置相关的头文件
#include "et_timer.h"    // 定时器相关操作的头文件
#include "string.h"      // 字符串处理的头文件
#include "stm32f1xx.h" // STM32F1系列的主头文件
#include "stm32f1xx_hal.h" // STM32硬件抽象层（HAL）头文件


/*******************************************************************************
 ************************ exported macros and struct types *********************
 ******************************************************************************/
#define OS_CPU_SR   uint32_t // 定义 CPU 状态寄存器的类型为 uint32_t

// 进入临界区的宏定义
#define enter_critical()        \
    do { cpu_sr = __get_PRIMASK(); __disable_irq();} while (0)

// 退出临界区的宏定义
#define exit_critical()         \
    do { __set_PRIMASK(cpu_sr);} while (0)

// 封装发布请求的宏，便于调用
#define ET_POST_REQUEST(type, req, size, routine, priv) \
    et_post_request_async(type, req, size, routine, priv)

// 请求类型枚举定义
typedef enum {
    ET_REQ_UART_RECV,  // UART 接收请求
    ET_REQ_UART_SEND,  // UART 发送请求
    ET_REQ_TYPE_MAX,   // 请求类型的最大值，用于边界检查
} et_req_type_t;

// UART 接收请求结构体
typedef struct {
    UART_HandleTypeDef *huart;
    uint16_t data_len;          // 接收到的数据长度
    uint8_t  data[64];          // 接收到的数据缓冲区，最大长度为 64 字节
} et_uart_recv_req_t;

// UART 发送请求结构体
typedef struct {
    uint8_t  opcode;            // 操作码，标识发送请求的类型
    uint16_t payload_len;       // 有效载荷长度
    uint8_t  payload_data[24];  // 发送的数据缓冲区，最大长度为 24 字节
} et_uart_send_req_t;

/*
 * 消息标准格式
 */
typedef void (*prot_rsp_cb_t)(const void *); // 定义协议响应回调函数指针类型

// 请求数据结构体
typedef struct {
    int             type;           // 请求类型
    prot_rsp_cb_t   cb_func;       // 完成后的回调函数
    const void     *priv;          // 传递给回调函数的私有数据
    union {
        int         data[3];       // 用于外设模块的通用数据
        et_uart_recv_req_t uart_recv_req; // UART 接收请求
        et_uart_send_req_t uart_send_req; // UART 发送请求
    } msg; // 使用联合体存储具体消息内容
} et_request_t;

// 产品初始化函数指针类型定义
typedef int (*et_prod_init_func_t)(void);

/*******************************************************************************
 ******************************* exported functions ****************************
 ******************************************************************************/
int et_post_request_async(int type, const void *req, unsigned int size,
                          prot_rsp_cb_t routine, const void *priv); // 异步请求发布函数

int et_task_init(void); // 任务初始化函数
void et_task_schedule(void); // 任务调度函数
void et_delay_ms(uint32_t ms); // 毫秒级延时函数
void et_delay_us(uint32_t us); // 微秒级延时函数

/*******************************************************************************
 ***************************  exported global variables ************************
 ******************************************************************************/
extern et_prod_init_func_t et_prod_init_tbl[]; // 产品初始化函数表的外部引用

#endif // _ET_OS_H_

/********************************* end of file ********************************/
