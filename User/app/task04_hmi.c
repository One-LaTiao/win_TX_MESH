#include "config.h"
#include "hmi_driver.h"
#include "cmd_process.h"
#include "hw_config.h"
#include "cmd_queue.h"




/*协议收发任务*/
static tmr_t tmr_start_task04_hmi;
static void task04_hmi(int timer_id, void *data);



uint8  cmd_buffer[CMD_MAX_SIZE];                                                     //指令缓存
//static uint16 current_screen_id = 0;                                                 //当前画面ID
//static int32 progress_value = 0;                                                     //进度条测试值
//static int32 test_value = 0;                                                         //测试值
//static uint8 update_en = 0;                                                          //更新标记
//static int32 meter_flag = 0;                                                         //仪表指针往返标志位
//static int32 num = 0;                                                                //曲线采样点计数
//static int sec = 1;                                                                  //时间秒
//static int32 curves_type = 0;                                                        //曲线标志位  0为正弦波，1为锯齿波                  
//static int32 second_flag=0;                                                          //时间标志位
//static int32 icon_flag = 0;                                                          //图标标志位
//static uint8 Select_H ;                                                              //滑动选择小时
//static uint8 Select_M ;                                                              //滑动选择分钟 
//static uint8 Last_H ;                                                                //上一个选择小时
//static uint8 Last_M;                                                                 //上一个选择分钟 
//static int32 Progress_Value = 0;                                                     //进度条的值 

uint8 num = 12;//设置温度更新次数


void UpdateUI(void);                                                                 //更新UI数据

void start_task04_hmi(void)
{
    start_rpt_tmr(&tmr_start_task04_hmi,task04_hmi, MS_TO_TICKS(20)); // 缩短采样周期
}

void task04_hmi(int timer_id, void *data)
{
  static uint8_t timeout = 0;
	qsize  size = 0;       
	size = queue_find_cmd(cmd_buffer,CMD_MAX_SIZE);                              //从缓冲区中获取一条指令         
	if(size>0&&cmd_buffer[1]!=0x07)                                              //接收到指令 ，及判断是否为开机提示
	{                                                                           
			ProcessMessage((PCTRL_MSG)cmd_buffer, size);                             //指令处理  
	}                  

	if(timeout++ >4)
	{
		UpdateUI();
		timeout = 0;
	}
}

/*! 
*  \brief  消息处理流程
*  \param msg 待处理消息
*  \param size 消息长度
*/
void ProcessMessage( PCTRL_MSG msg, uint16 size )
{
    uint8 cmd_type = msg->cmd_type;                                                  //指令类型
    uint8 ctrl_msg = msg->ctrl_msg;                                                  //消息的类型
    uint8 control_type = msg->control_type;                                          //控件类型
    uint16 screen_id = PTR2U16(&msg->screen_id);                                     //画面ID
    uint16 control_id = PTR2U16(&msg->control_id);                                   //控件ID
    uint32 value = PTR2U32(msg->param);                                              //数值


    switch(cmd_type)
    {  
    case NOTIFY_TOUCH_PRESS:                                                        //触摸屏按下
    case NOTIFY_TOUCH_RELEASE:                                                      //触摸屏松开
        NotifyTouchXY(cmd_buffer[1],PTR2U16(cmd_buffer+2),PTR2U16(cmd_buffer+4)); 
        break;                                                                    
    case NOTIFY_WRITE_FLASH_OK:                                                     //写FLASH成功
        NotifyWriteFlash(1);                                                      
        break;                                                                    
    case NOTIFY_WRITE_FLASH_FAILD:                                                  //写FLASH失败
        NotifyWriteFlash(0);                                                      
        break;                                                                    
    case NOTIFY_READ_FLASH_OK:                                                      //读取FLASH成功
        NotifyReadFlash(1,cmd_buffer+2,size-6);                                     //去除帧头帧尾
        break;                                                                    
    case NOTIFY_READ_FLASH_FAILD:                                                   //读取FLASH失败
        NotifyReadFlash(0,0,0);                                                   
        break;                                                                    
    case NOTIFY_READ_RTC:                                                           //读取RTC时间
        NotifyReadRTC(cmd_buffer[2],cmd_buffer[3],cmd_buffer[4],cmd_buffer[5],cmd_buffer[6],cmd_buffer[7],cmd_buffer[8]);
        break;
    case NOTIFY_CONTROL:
        {
            if(ctrl_msg==MSG_GET_CURRENT_SCREEN)                                    //画面ID变化通知
            {
                NotifyScreen(screen_id);                                            //画面切换调动的函数
            }
            else
            {
                switch(control_type)
                {
                case kCtrlButton:                                                   //按钮控件
                    NotifyButton(screen_id,control_id,msg->param[1]);                  
                    break;                                                             
                case kCtrlText:                                                     //文本控件
                    NotifyText(screen_id,control_id,msg->param);                       
                    break;                                                             
                case kCtrlProgress:                                                 //进度条控件
                    NotifyProgress(screen_id,control_id,value);                        
                    break;                                                             
                case kCtrlSlider:                                                   //滑动条控件
                    NotifySlider(screen_id,control_id,value);                          
                    break;                                                             
                case kCtrlMeter:                                                    //仪表控件
                    NotifyMeter(screen_id,control_id,value);                           
                    break;                                                             
                case kCtrlMenu:                                                     //菜单控件
                    NotifyMenu(screen_id,control_id,msg->param[0],msg->param[1]);      
                    break;                                                              
                case kCtrlSelector:                                                 //选择控件
                    NotifySelector(screen_id,control_id,msg->param[0]);                
                    break;                                                              
                case kCtrlRTC:                                                      //倒计时控件
                    NotifyTimer(screen_id,control_id);
                    break;
                default:
                    break;
                }
            } 
            break;  
        } 
    case NOTIFY_HandShake:                                                          //握手通知                                                     
        NOTIFYHandShake();
        break;
    default:
        break;
    }
}
/*! 
*  \brief  握手通知
*/
void NOTIFYHandShake(void)
{
   SetButtonValue(3,2,1);
}

/*! 
*  \brief  画面切换通知
*  \details  当前画面改变时(或调用GetScreen)，执行此函数
*  \param screen_id 当前画面ID
*/
void NotifyScreen(uint16 screen_id)
{
    //TODO: 添加用户代码


}

/*! 
*  \brief  触摸坐标事件响应
*  \param press 1按下触摸屏，3松开触摸屏
*  \param x x坐标
*  \param y y坐标
*/
void NotifyTouchXY(uint8 press,uint16 x,uint16 y)
{ 
    //TODO: 添加用户代码
}

/*! 
*  \brief  更新数据
*  注意：若串口屏没有RTC 请先看下工程画面ID，对应工功能画面的编号与下面数据刷新的一致
*/ 
void UpdateUI()
{
//	SetTextInt32(0,7,88,0,2);
//	SetTextValue(0,7,"叁");

//	for(uint8_t i =0;i<12;i++)
//	{
//		SetTextInt32(0,i+13,temp_value[i],0,2);//实时温度
//	//		if(flag_heat[i] == 0) SetTextValue(0,i+37,"关"); else SetTextValue(0,i+37,"开");//实时温度
//	}
//	SetTextInt32(0,25, target_temp,0,2);//设定温度
//	SetTextInt32(1,3,bcd_value[0],0,2);//地址

	
}


/*! 
*  \brief  按钮控件通知
*  \details  当按钮状态改变(或调用GetControlValue)时，执行此函数
*  \param screen_id 画面ID
*  \param control_id 控件ID
*  \param state 按钮状态：0弹起，1按下
*/
void NotifyButton(uint16 screen_id, uint16 control_id, uint8  state)
{
	switch(screen_id)
	{
		case 0: 
			switch(control_id){/*总控*/
				case 10:
					if(state){/*广播全部打开*/
						CMD_sta = 1;
						CMD_cmd = 1;
						CMD_id = 0xff;
					}
				break;				
				case 11:
					if(state){/*广播全部关闭*/
						CMD_sta = 1;
						CMD_cmd = 2;
						CMD_id = 0xff;
					}
				break;				
				case 12:
					if(state){/*广播全部停止*/
						CMD_sta = 1;
						CMD_cmd = 3;
						CMD_id = 0xff;
					}
				break;
				default:
					break;
			}
			break;
		case 1: 
			switch(control_id){/*东面窗开关*/
				case 10:
					if(state){/*打开*/
						CMD_sta = 1;
						CMD_cmd = 1;
						CMD_id = 0x01;
					}
				break;				
				case 11:
					if(state){/*关闭*/
						CMD_sta = 1;
						CMD_cmd = 2;
						CMD_id = 0x01;
					}
				break;				
				case 12:
					if(state){/*打开*/
						CMD_sta = 1;
						CMD_cmd = 1;
						CMD_id = 0x02;
					}
				break;
				case 13:
					if(state){/*关闭*/
						CMD_sta = 1;
						CMD_cmd = 2;
						CMD_id = 0x02;
					}
				break;				
				case 14:
					if(state){/*打开*/
						CMD_sta = 1;
						CMD_cmd = 1;
						CMD_id = 0x03;
					}
				break;				
				case 15:
					if(state){/*关闭*/
						CMD_sta = 1;
						CMD_cmd = 2;
						CMD_id = 0x03;
					}
				break;
				case 20:
					if(state){/*广播全部停止*/
						CMD_sta = 1;
						CMD_cmd = 3;
						CMD_id = 0xff;
					}
				break;				

				default:
					break;
			}
			break;
		case 2: 
			switch(control_id){/*西面窗开关*/
				case 10:
					if(state){/*打开*/
						CMD_sta = 1;
						CMD_cmd = 1;
						CMD_id = 0x04;
					}
					break;				
				case 11:
					if(state){/*关闭*/
						CMD_sta = 1;
						CMD_cmd = 2;
						CMD_id = 0x04;
					}
				break;				
				case 12:
					if(state){/*打开*/
						CMD_sta = 1;
						CMD_cmd = 1;
						CMD_id = 0x05;
					}
					break;
				case 13:
					if(state){/*关闭*/
						CMD_sta = 1;
						CMD_cmd = 2;
						CMD_id = 0x05;
					}
					break;				
				case 14:
					if(state){/*打开*/
						CMD_sta = 1;
						CMD_cmd = 1;
						CMD_id = 0x06;
					}
					break;				
				case 15:
					if(state){/*关闭*/
						CMD_sta = 1;
						CMD_cmd = 2;
						CMD_id = 0x06;
					}
					break;
				case 20:
					if(state){/*广播全部停止*/
						CMD_sta = 1;
						CMD_cmd = 3;
						CMD_id = 0xff;
					}
					break;				

				default:
					break;
			}
			break;
		case 3: 
			switch(control_id){/*南面窗开关*/
				case 10:
					if(state){/*打开*/
						CMD_sta = 1;
						CMD_cmd = 1;
						CMD_id = 0x07;
					}
					break;				
				case 11:
					if(state){/*关闭*/
						CMD_sta = 1;
						CMD_cmd = 2;
						CMD_id = 0x07;
					}
				break;				
				case 12:
					if(state){/*打开*/
						CMD_sta = 1;
						CMD_cmd = 1;
						CMD_id = 0x08;
					}
					break;
				case 13:
					if(state){/*关闭*/
						CMD_sta = 1;
						CMD_cmd = 2;
						CMD_id = 0x08;
					}
					break;				
				case 14:
					if(state){/*打开*/
						CMD_sta = 1;
						CMD_cmd = 1;
						CMD_id = 0x09;
					}
					break;				
				case 15:
					if(state){/*关闭*/
						CMD_sta = 1;
						CMD_cmd = 2;
						CMD_id = 0x09;
					}
					break;
				case 16:
					if(state){/*打开*/
						CMD_sta = 1;
						CMD_cmd = 1;
						CMD_id = 0x0a;
					}
					break;				
				case 17:
					if(state){/*关闭*/
						CMD_sta = 1;
						CMD_cmd = 2;
						CMD_id = 0x0a;
					}
					break;
				case 20:
					if(state){/*广播全部停止*/
						CMD_sta = 1;
						CMD_cmd = 3;
						CMD_id = 0xff;
					}
					break;				

				default:
					break;
			}
			break;
		case 4: 
			switch(control_id){/*南面窗开关*/
				case 10:
					if(state){/*打开*/
						CMD_sta = 1;
						CMD_cmd = 1;
						CMD_id = 0x0b;
					}
					break;				
				case 11:
					if(state){/*关闭*/
						CMD_sta = 1;
						CMD_cmd = 2;
						CMD_id = 0x0b;
					}
				break;				
				case 12:
					if(state){/*打开*/
						CMD_sta = 1;
						CMD_cmd = 1;
						CMD_id = 0x0c;
					}
					break;
				case 13:
					if(state){/*关闭*/
						CMD_sta = 1;
						CMD_cmd = 2;
						CMD_id = 0x0c;
					}
					break;				
				case 14:
					if(state){/*打开*/
						CMD_sta = 1;
						CMD_cmd = 1;
						CMD_id = 0x0d;
					}
					break;				
				case 15:
					if(state){/*关闭*/
						CMD_sta = 1;
						CMD_cmd = 2;
						CMD_id = 0x0d;
					}
					break;
				case 16:
					if(state){/*打开*/
						CMD_sta = 1;
						CMD_cmd = 1;
						CMD_id = 0x0e;
					}
					break;				
				case 17:
					if(state){/*关闭*/
						CMD_sta = 1;
						CMD_cmd = 2;
						CMD_id = 0x0e;
					}
					break;
				case 20:
					if(state){/*广播全部停止*/
						CMD_sta = 1;
						CMD_cmd = 3;
						CMD_id = 0xff;
					}
					break;				
				default:
					break;
			}
			break;
		case 5: 
			switch(control_id){/*设置界面*/
				case 10:
					if(state){/*打开*/
						CMD_sta = 1;
						CMD_cmd = 1;
						CMD_id = 0xff;
					}
					break;							
				default:
					break;
			}
			break;		
		default:
			break;
	}
}

/*! 
*  \brief  文本控件通知
*  \details  当文本通过键盘更新(或调用GetControlValue)时，执行此函数
*  \details  文本控件的内容以字符串形式下发到MCU，如果文本控件内容是浮点值，
*  \details  则需要在此函数中将下发字符串重新转回浮点值。
*  \param screen_id 画面ID
*  \param control_id 控件ID
*  \param str 文本控件内容
*/
void NotifyText(uint16 screen_id, uint16 control_id, uint8 *str)
{
   float value=0;                                                            
   sscanf((const char*)str,"%f",&value);	
	switch(screen_id)
	{
		case 0: 
			break;
		default:
			break;
		
	}
}                                                                                

/*!                                                                              
*  \brief  进度条控件通知                                                       
*  \details  调用GetControlValue时，执行此函数                                  
*  \param screen_id 画面ID                                                      
*  \param control_id 控件ID                                                     
*  \param value 值                                                              
*/                                                                              
void NotifyProgress(uint16 screen_id, uint16 control_id, uint32 value)           
{  
  
}                                                                                

/*!                                                                              
*  \brief  滑动条控件通知                                                       
*  \details  当滑动条改变(或调用GetControlValue)时，执行此函数                  
*  \param screen_id 画面ID                                                      
*  \param control_id 控件ID                                                     
*  \param value 值                                                              
*/                                                                              
void NotifySlider(uint16 screen_id, uint16 control_id, uint32 value)             
{                                                             

}

/*! 
*  \brief  仪表控件通知
*  \details  调用GetControlValue时，执行此函数
*  \param screen_id 画面ID
*  \param control_id 控件ID
*  \param value 值
*/
void NotifyMeter(uint16 screen_id, uint16 control_id, uint32 value)
{
    //TODO: 添加用户代码
}

/*! 
*  \brief  菜单控件通知
*  \details  当菜单项按下或松开时，执行此函数
*  \param screen_id 画面ID
*  \param control_id 控件ID
*  \param item 菜单项索引
*  \param state 按钮状态：0松开，1按下
*/
void NotifyMenu(uint16 screen_id, uint16 control_id, uint8 item, uint8 state)
{
    //TODO: 添加用户代码
}

/*! 
*  \brief  选择控件通知
*  \details  当选择控件变化时，执行此函数
*  \param screen_id 画面ID
*  \param control_id 控件ID
*  \param item 当前选项
*/
void NotifySelector(uint16 screen_id, uint16 control_id, uint8  item)
{


}

/*! 
*  \brief  定时器超时通知处理
*  \param screen_id 画面ID
*  \param control_id 控件ID
*/
void NotifyTimer(uint16 screen_id, uint16 control_id)
{

}

/*! 
*  \brief  读取用户FLASH状态返回
*  \param status 0失败，1成功
*  \param _data 返回数据
*  \param length 数据长度
*/
void NotifyReadFlash(uint8 status,uint8 *_data,uint16 length)
{
    //TODO: 添加用户代码
}

/*! 
*  \brief  写用户FLASH状态返回
*  \param status 0失败，1成功
*/
void NotifyWriteFlash(uint8 status)
{
    //TODO: 添加用户代码
}

/*! 
*  \brief  读取RTC时间，注意返回的是BCD码
*  \param year 年（BCD）
*  \param month 月（BCD）
*  \param week 星期（BCD）
*  \param day 日（BCD）
*  \param hour 时（BCD）
*  \param minute 分（BCD）
*  \param second 秒（BCD）
*/
void NotifyReadRTC(uint8 year,uint8 month,uint8 week,uint8 day,uint8 hour,uint8 minute,uint8 second)
{

}




