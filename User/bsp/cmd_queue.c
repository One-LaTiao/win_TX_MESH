/*
--------------------------------------------------------------------------------------
使用必读
cmd_queue.c中共5个函数：清空指令数据queue_reset()、从串口添加指令数据queue_push()、
从队列中取一个数据queue_pop().获取队列中有效数据个数queue_size()、从指令队列中取出一条完整的指令queue_find_cmd（）
若移植到其他平台，需要修改底层寄存器设置,但禁止修改函数名称，否则无法与HMI驱动库(hmi_driver.c)匹配。
--------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------*/
#include "cmd_queue.h"



#define CMD_HEAD 0XEE                                                  //帧头
#define CMD_TAIL 0XFFFCFFFF                                            //帧尾

static uint8_t calculateXOR(const uint8_t *data);

typedef struct _QUEUE                                             
{                                                                 
    qsize _head;                                                       //队列头
    qsize _tail;                                                       //队列尾
    qdata _data[QUEUE_MAX_SIZE];                                       //队列数据缓存区
}QUEUE;                                                           

static QUEUE que = {0,0,0}; //指令队列触摸屏
static uint32_t cmd_state = 0;                                         //队列帧尾检测状态
static qsize cmd_pos = 0;                                              //当前指令指针位置



#define UART_CMD_HEAD 0X7B                                             		//帧头
#define UART_CMD_ADDR 0X01	//	地址
#define UART_CMD_LEN  0x09  //长度

static QUEUE uart_que = {0,0,0};
static uint32_t uart_cmd_state = 0;                                         //队列帧尾检测状态
static qsize uart_cmd_pos = 0;                                              //当前指令指针位置



/*
* 函数名 :CRC16
* 描述 : 计算CRC16
* 输入 : puchMsg---数据地址,usDataLen---数据长度
* 输出 : 校验值
*/
uint16_t CRC16_MudBus(uint8_t *puchMsg, uint8_t usDataLen)
{
	
	uint16_t uCRC = 0xffff;//CRC寄存器
	
	for(uint8_t num=0;num<usDataLen;num++){
		uCRC = (*puchMsg++)^uCRC;//把数据与16位的CRC寄存器的低8位相异或，结果存放于CRC寄存器。
		for(uint8_t x=0;x<8;x++){	//循环8次
			if(uCRC&0x0001){	//判断最低位为：“1”
				uCRC = uCRC>>1;	//先右移
				uCRC = uCRC^0xA001;	//再与0xA001异或
			}else{	//判断最低位为：“0”
				uCRC = uCRC>>1;	//右移
			}
		}
	}
	return uCRC;//返回CRC校验值
}


/*! 
*  \brief  清空指令数据
*/
void queue_reset()
{
	que._head = que._tail = 0;
	cmd_pos = cmd_state = 0;
}
/*! 
* \brief  添加指令数据
* \detial 串口接收的数据，通过此函数放入指令队列 
*  \param  _data 指令数据
*/
void queue_push(qdata _data)
{
	qsize pos = (que._head+1)%QUEUE_MAX_SIZE;
	if(pos!=que._tail)                                                //非满状态
	{
		que._data[que._head] = _data;
		que._head = pos;
	}
}

//从队列中取一个数据
static void queue_pop(qdata* _data)
{
    if(que._tail!=que._head)                                          //非空状态
    {
        *_data = que._data[que._tail];
        que._tail = (que._tail+1)%QUEUE_MAX_SIZE;
    }
}

//获取队列中有效数据个数
static qsize queue_size()
{
    return ((que._head+QUEUE_MAX_SIZE-que._tail)%QUEUE_MAX_SIZE);
}
/*! 
*  \brief  从指令队列中取出一条完整的指令
*  \param  cmd 指令接收缓存区
*  \param  buf_len 指令接收缓存区大小
*  \return  指令长度，0表示队列中无完整指令
*控制面板接收到  
*/


qsize queue_find_cmd(qdata *buffer,qsize buf_len)
{
    qsize cmd_size = 0;
    qdata _data = 0;

    while(queue_size()>0)
    {
        //取一个数据
        queue_pop(&_data);

        if(cmd_pos==0&&_data!=CMD_HEAD)                               //指令第一个字节必须是帧头，否则跳过
        {
            continue;
        }
        //    LED2_ON;
        if(cmd_pos<buf_len)                                           //防止缓冲区溢出
            buffer[cmd_pos++] = _data;

        cmd_state = ((cmd_state<<8)|_data);                           //拼接最后4个字节，组成一个32位整数

        //最后4个字节与帧尾匹配，得到完整帧
        if(cmd_state==CMD_TAIL)
        {
            //LED2_ON;
            cmd_size = cmd_pos;                                       //指令字节长度
            cmd_state = 0;                                            //重新检测帧尾巴
            cmd_pos = 0;                                              //复位指令指针

#if(CRC16_ENABLE)
            //去掉指令头尾EE，尾FFFCFFFF共计5个字节，只计算数据部分CRC
            if(!CheckCRC16(buffer+1,cmd_size-5))                      //CRC校验
                return 0;

            cmd_size -= 2;                                            //去掉CRC16（2字节）
#endif
            return cmd_size;
        }
    }
    return 0;                                                         //没有形成完整的一帧
}



















/*! 
*  \brief  清空指令数据
*/
void uart_queue_reset()
{
    uart_que._head = uart_que._tail = 0;
    uart_cmd_pos = uart_cmd_state = 0;
}
/*! 
* \brief  添加指令数据
* \detial 串口接收的数据，通过此函数放入指令队列 
*  \param  _data 指令数据
*/
void uart_queue_push(qdata _data)
{
    qsize pos = (uart_que._head+1)%QUEUE_MAX_SIZE;
    if(pos!=uart_que._tail)                                                //非满状态
    {
        uart_que._data[uart_que._head] = _data;
        uart_que._head = pos;
    }
}

//从队列中取一个数据
static void uart_queue_pop(qdata* _data)
{
    if(uart_que._tail!=uart_que._head)                                          //非空状态
    {
        *_data = uart_que._data[uart_que._tail];
        uart_que._tail = (uart_que._tail+1)%QUEUE_MAX_SIZE;
    }
}

//获取队列中有效数据个数
static qsize uart_queue_size()
{
    return ((uart_que._head+QUEUE_MAX_SIZE-uart_que._tail)%QUEUE_MAX_SIZE);
}
/*! 
*  \brief  从指令队列中取出一条完整的指令
*  \param  cmd 指令接收缓存区
*  \param  buf_len 指令接收缓存区大小
*  \return  指令长度，0表示队列中无完整指令
*控制面板接收到  
* 7B 7B 09 10 03 01 00 00 00 00 0F 7D 7D
*/

qsize uart_queue_find_cmd(qdata *buffer,qsize buf_len)
{
	qdata _data = 0;
	uint16_t crc_data = 0;

	while(uart_queue_size()>0)
	{
		//取一个数据
		uart_queue_pop(&_data);
		if(((uart_cmd_pos == 0) && (_data==UART_CMD_HEAD))||((uart_cmd_pos == 1) && (_data == UART_CMD_HEAD)))//判断是不是帧头
		{
			buffer[uart_cmd_pos++] = _data;//启动接收
			continue;
		}
		
		if((uart_cmd_pos == 1) && (_data != UART_CMD_HEAD))
		{
			uart_cmd_pos =0;
			return 0;
		}
		
		if(uart_cmd_pos >= 2)//判断已收到准确的头帧
		{
			buffer[uart_cmd_pos++] = _data;
			if(uart_cmd_pos >= 13)
			{									
				crc_data = calculateXOR(buffer);
				if(buffer[10] == crc_data)
				{			
					uart_cmd_state = uart_cmd_pos;
					uart_cmd_pos = 0;
					return uart_cmd_state;
				}
				else
				{
					uart_cmd_pos = 0;
					return 0;
				}
			}
		}
	}
    return 0;                                                       
}


int fputc(int ch, FILE *f)
{
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xffff);
  return ch;
}
 

int fgetc(FILE *f)
{
  uint8_t ch = 0;
  HAL_UART_Receive(&huart1, &ch, 1, 0xffff);
  return ch;
}


static uint8_t calculateXOR(const uint8_t *data)
{
    // 从第3字节开始计算8个字节的异或值 (跳过前两个0x7B)
    uint8_t xorValue = 0;
    for (uint8_t pos = 2; pos <= data[2]; pos++) {
        xorValue ^= data[pos];
    }
    return xorValue;
}









