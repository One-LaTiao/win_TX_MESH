#include "config.h"
#include "usart.h"

/*状态指示灯*/
static tmr_t tmr_start_led;
static void start_led(void);




volatile uint8_t CMD_sta = 0;;
volatile uint8_t CMD_cmd = 0;
volatile uint8_t CMD_id = 0;


static int bsp_init(void)
{
	bsp_config();

  return 0;
}

static int app_init(void)
{
//    log_info("\r\nFirmware name: %s \r\n", FW_NAME);
//    log_info("version: %d.%d.%d \r\n", FW_VERSION_MAJOR, FW_VERSION_MINOR, FW_VERSION_patch);
	
	/*运行指示灯*/
	start_led();
	start_task04_hmi();
  return 0;
}

et_prod_init_func_t et_prod_init_tbl[] = {
    bsp_init,
    app_init,
    NULL,     /* !! NULL MUST be here as a sentinal  */
};



/**
* @brief 计算校验和
* @param cmd: 命令字节
* @param data_len: 数据长度
* @param data: 数据指针
* @retval 校验和
*/
uint8_t Calculate_Checksum(uint8_t *data)
{
    uint8_t checksum = 0;
    // 从第3字节开始计算8个字节的异或值 (跳过前两个0x7B)
    for (uint8_t pos = 2; pos <= data[2]; pos++) {
        checksum ^= data[pos];
    }

    return checksum;
}



/*状态指示灯处理*/
static void tmr_start_led_proc(int timer_id, void *data)
{
	uint8_t pData[16]={0};
	HAL_GPIO_TogglePin(led_GPIO_Port,led_Pin);
	if(CMD_sta)
	{
		pData[0] = 0x7B;
		pData[1] = 0x7B;
		pData[2] = 0x09;//数量
		pData[3] = CMD_id;//从机地址
		pData[4] = 0x03;//读写
		pData[5] = 0x01;//寄存器地址
		pData[6] = CMD_id;
		pData[7] = 0x01;
		pData[8] = CMD_cmd;
		pData[9] = 0x01;
		pData[10] = Calculate_Checksum(pData);//校验
		pData[11] = 0x7D;
		pData[12] = 0x7D;
		HAL_UART_Transmit_DMA(&huart2,pData,13);
		CMD_sta = 0;
		CMD_cmd = 0;
		CMD_id = 0;
	}



}
static void start_led(void)
{
	start_rpt_tmr(&tmr_start_led, tmr_start_led_proc, MS_TO_TICKS(1));
}
