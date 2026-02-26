#include "fm24cl04.h"
#include "i2c.h"

#define SPEED_ADDR 0x0002
#define CURRENT_ADDR 0x0004

/*目标温度写入*/
void temp_write(uint8_t data)
{
	uint8_t buf[3] ={0x00,0x02};
	buf[2] = data;
	HAL_I2C_Master_Transmit(&hi2c1,FM24CL16_Write_1,buf,3,1000);
	HAL_Delay(2);
}

/*目标温度读出*/
void temp_read(uint8_t *data)
{
	uint8_t buf[3] ={0x00,0x02};
	HAL_I2C_Master_Transmit(&hi2c1,FM24CL16_Write_1,buf,2,1000);
	HAL_Delay(2);
	HAL_I2C_Master_Receive(&hi2c1,FM24CL16_Write_1,data,1,1000);
	HAL_Delay(2);

}


/*超温保护写入*/
void over_temp_write(uint8_t data)
{
	uint8_t buf[3] ={0x00,0x04};
	buf[2] = data;
	HAL_I2C_Master_Transmit(&hi2c1,FM24CL16_Write_1,buf,3,1000);
	HAL_Delay(2);
}

/*超温保护读出*/
void over_temp_read(uint8_t *data)
{
	uint8_t buf[3] ={0x00,0x04};
	HAL_I2C_Master_Transmit(&hi2c1,FM24CL16_Write_1,buf,2,1000);
	HAL_Delay(2);
	HAL_I2C_Master_Receive(&hi2c1,FM24CL16_Write_1,data,1,1000);
	HAL_Delay(2);

}


