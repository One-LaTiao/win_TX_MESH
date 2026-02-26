/**
 *******************************************************************************
 * @file    et_os.c
 * @author  yeelight
 * @version 1.0.0
 * @date    2022-11-11
 * @brief   V1 2022-11-11
 *          create
 *
 *******************************************************************************
 * @attention
 * 注意事项
 *
 *******************************************************************************
 */

/*******************************************************************************
 * include files
 ******************************************************************************/
#include "et_os.h"       // 自定义操作系统头文件
#include "main.h"
#include "usart.h"


/*******************************************************************************
 * private define macro and struct types
 ******************************************************************************/
#define TRACE_ENTER()   log_info("[ et ] enter: %s\r\n", __FUNCTION__) // 进入函数时记录日志

#define YGLC_MAX_REQUSTS         18 // 定义最大请求数
#define YGLC_POOL_ITEMS          (YGLC_MAX_REQUSTS) // 定义池项数量

// 定义用于缓冲请求的结构体
typedef struct {
    list_head_t    self; // 链表节点，用于在请求链表中链接
    int            occupied; // 标志当前缓冲区是否被占用
    uint8_t        buffer[sizeof(et_request_t)]; // 用于存储请求数据的缓冲区
} et_buffer_t;

/*******************************************************************************
 * private function prototypes
 ******************************************************************************/
//static void et_proc_request(et_request_t *req); // 处理请求的函数原型
static void et_req_handler(void); // 请求处理循环的函数原型

/*******************************************************************************
 * private variables
 ******************************************************************************/
static et_buffer_t et_buffer_pool[YGLC_POOL_ITEMS] = {0}; // 定义请求缓冲池，初始化为全0
static KLIST_HEAD(req_head); // 定义请求链表头

/*******************************************************************************
 *******************************************************************************
 * private application code, functions definitions
 *******************************************************************************
 ******************************************************************************/
static et_buffer_t *et_buffer_pool_alloc(void)
{
    // 在缓冲池中分配一个空闲的缓冲区
    for (uint32_t index = 0; index < YGLC_POOL_ITEMS; index++) {
        if (!et_buffer_pool[index].occupied) { // 如果该缓冲区未占用
            et_buffer_pool[index].occupied = true; // 标记为占用
            return &et_buffer_pool[index]; // 返回该缓冲区的指针
        }
    }
    return NULL; // 如果没有空闲的缓冲区，返回 NULL
}

static void et_buffer_pool_release(et_buffer_t *et_buffer)
{
    // 释放占用的缓冲区
    et_buffer->occupied = false; // 标记为未占用
}

void et_task_schedule(void)
{
    proc_timer(); // 调用处理定时器任务
    et_req_handler(); // 处理请求的函数
}

/**
 * @brief The App Entry function应用程序入口函数
 *
 * @return int 返回初始化结果，0为成功，负值为失败
 */
static int et_prod_init(void)
{
    et_prod_init_func_t *fn; // 初始化功能函数指针
    int rc;

    // 遍历设备初始化函数表并调用每个函数
    for (fn = &et_prod_init_tbl[0]; *fn != NULL; fn++) {
        rc = (*fn)(); // 调用初始化函数
        if (rc < 0) { // 如果返回值小于0，表示初始化失败
            log_err("yl product specific init failed.\r\n");
            return rc; // 返回错误代码
        }
    }

    return 0; // 成功
}

int et_task_init(void)
{
    timer_init(); // 初始化定时器
    int rc = et_prod_init(); // 初始化产品相关内容
    if (rc != 0) {
        log_err("et product init err!!\r\n");
        return -1; // 若初始化失败，返回错误
    }

    return 0; // 成功
}

/**
 * @brief Get msg from et queue/list从事件任务队列 / 列表中获取消息
 *
 * 处理请求列表中的消息
 */
static void et_req_handler(void)
{
again: // 标签，用于重试处理请求

    if (list_empty(&req_head)) { // 检查请求链表是否为空
        return; // 如果为空，返回
    }

    // 获取请求缓冲区的第一个元素
    et_buffer_t *et_buffer = list_first_entry(&req_head, et_buffer_t, self);
    et_request_t *et_req = (et_request_t *)(et_buffer->buffer); // 解析请求

//    et_proc_request(et_req); // 处理当前请求

    if (et_req->cb_func != NULL) { // 若请求中设置了回调函数
        et_req->cb_func((void *)et_req->priv); // 调用回调函数
    }

    // 检查是否可以从链表中删除
    if (et_buffer->self.next != NULL && et_buffer->self.prev != NULL) {
        list_del(&(et_buffer->self)); // 从链表中删除当前请求
        et_buffer_pool_release(et_buffer); // 释放缓冲区
    }

    goto again; // 继续处理下一个请求
}

/**
 * @brief Post msg to et queue/list从事件任务队列 / 列表中获取消息
 *
 * @param type 请求类型
 * @param req 请求数据指针
 * @param size 请求数据大小
 * @param routine 请求完成后的回调函数
 * @param priv 传递给回调函数的私有数据
 * @return int 返回结果，0表示成功，负值表示失败
 */
int et_post_request_async(int type, const void *req, unsigned int size,
                          prot_rsp_cb_t routine, const void *priv)
{
    et_buffer_t *et_buffer = et_buffer_pool_alloc(); // 分配请求缓冲区
    if (NULL == et_buffer) {
        log_err("No et buffer, req:%d is dropped!\r\n", type); // 若无可用缓冲区，记录错误
        return -1; // 返回错误
    }

    et_request_t *et_req = (et_request_t *)et_buffer->buffer; // 获取请求
    et_req->type = type; // 设置请求类型
    et_req->cb_func = routine; // 设置回调函数
    et_req->priv = priv; // 设置私有数据

    if (req) {
        memmove(&(et_req->msg), req, size); // 拷贝请求数据
    }

    // 添加请求到链表
    list_add_tail(&(et_buffer->self), &req_head); // 将新请求添加到请求链表尾部

    return 0; // 返回成功
}

/**
 * @brief Message distribution消息分发
 //请求
	memcpy(uart_recv_data.data, &g_ucaRcvBuff[0][0], Size);
	ET_POST_REQUEST(ET_REQ_UART_RECV, &uart_recv_data, sizeof(et_uart_recv_req_t), NULL, NULL);添加请求
	HAL_UARTEx_ReceiveToIdle_DMA(&huart_motor_geduan, &g_ucaRcvBuff[0][0], RCV_BUFF_SIZE);
 * @param req 查收到的请求
 */
//static void et_proc_request(et_request_t *req)
//{
//    /* log_info("Exec et req, req: %d\r\n", req->type); */

//    switch (req->type) {
//    case ET_REQ_UART_RECV:
//        if (req->msg.uart_recv_req.huart == &uart_1) {
//            /*收到头枕编码器数据*/
////            uart_encoder_touzhen_recv_data(req->msg.uart_recv_req.data, req->msg.uart_recv_req.data_len);
//        } 
//				else if (req->msg.uart_recv_req.huart == &huart_ctrl) {
//            uart_ctrl_recv_data(req->msg.uart_recv_req.data, req->msg.uart_recv_req.data_len);
//        } else if (req->msg.uart_recv_req.huart == &huart_motor_geduan) {
//            uart_geduan_recv_data(req->msg.uart_recv_req.data, req->msg.uart_recv_req.data_len);
//        }
//        break;

//    case ET_REQ_UART_SEND:
//        //uart_apec_send_data(req->msg.uart_send_req.opcode, req->msg.uart_send_req.payload_data, req->msg.uart_send_req.payload_len);
//        break;

//    default:
//        log_err("et: not support the YGLC request id\n");
//        break;
//    }
//}

/**
 * @brief Delay execution for a specified number of milliseconds
 *
 * @param ms 延迟时间（毫秒）
 */
void et_delay_ms(uint32_t ms)
{
    uint32_t tickstart = 0;
    tickstart = get_sys_time_ms(); // 获取当前系统时间

    // 持续循环，直到时间到达指定延迟
    while ((get_sys_time_ms() - tickstart) < ms);
}

/**
 * @brief Delay execution for a specified number of microseconds
 *
 * @param us 延迟时间（微秒）
 */
void et_delay_us(uint32_t us)
{
    uint32_t tickstart = 0;
    tickstart = get_sys_time_micro(); // 获取当前系统微秒时间

    // 持续循环，直到时间到达指定延迟
    while ((get_sys_time_micro() - tickstart) < us);
}



/********************************* end of file ********************************/
