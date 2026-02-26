#include "config.h"

/*状态指示灯*/
static tmr_t tmr_start_led;
static void start_led(void);

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


/*状态指示灯处理*/
static void tmr_start_led_proc(int timer_id, void *data)
{
	if(HAL_GPIO_ReadPin(key_1_GPIO_Port,key_1_Pin) == GPIO_PIN_RESET){
		tx_data = 0x000000AAU;
		tx_sta = 1;
	}
	if(HAL_GPIO_ReadPin(key_2_GPIO_Port,key_2_Pin) == GPIO_PIN_RESET){
		tx_data = 0x000000BBU;
		tx_sta = 1;
	}
	if((HAL_GPIO_ReadPin(key_2_GPIO_Port,key_2_Pin) == GPIO_PIN_RESET) && (HAL_GPIO_ReadPin(key_1_GPIO_Port,key_1_Pin) == GPIO_PIN_RESET)){
		tx_data = 0x000000CCU;
		tx_sta = 1;
	}
	HAL_GPIO_TogglePin(led_GPIO_Port,led_Pin);



}
static void start_led(void)
{
	start_rpt_tmr(&tmr_start_led, tmr_start_led_proc, MS_TO_TICKS(100));
}
