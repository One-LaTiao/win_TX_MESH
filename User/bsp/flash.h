#ifndef __FLASH_H__
#define __FLASH_H__


#include "main.h"


//用户根据自己的需要设置
#define STM32_FLASH_SIZE 	512 	 		//所选STM32的FLASH容量大小(单位为K)
#define STM32_FLASH_WREN 	1              	//使能FLASH写入(0，不使能;1，使能)
#define FLASH_WAITETIME  	5000          	//FLASH等待超时时间

//FLASH起始地址
#define STM32_FLASH_BASE 0x08000000 		//STM32 FLASH的起始地址

uint8_t STMFLASH_GetStatus(void);				  //获得状态
uint8_t STMFLASH_WaitDone(uint16_t time);				  //等待操作结束
uint8_t STMFLASH_ErasePage(uint32_t paddr);			  //擦除页
uint8_t STMFLASH_WriteHalfWord(uint32_t faddr, uint32_t dat);//写入半字
uint32_t STMFLASH_ReadHalfWord(uint32_t faddr);		  //读出半字  

void STMFLASH_WriteLenByte(uint32_t WriteAddr,uint32_t DataToWrite,uint16_t Len);	//指定地址开始写入指定长度的数据
uint32_t STMFLASH_ReadLenByte(uint32_t ReadAddr,uint16_t Len);						//指定地址开始读取指定长度数据
void STMFLASH_Write(uint32_t WriteAddr,const uint16_t *pBuffer,uint16_t NumToWrite);		//从指定地址开始写入指定长度的数据
void STMFLASH_Read(uint32_t ReadAddr,uint16_t *pBuffer,uint16_t NumToRead);   		//从指定地址开始读出指定长度的数据





//0-49dianji canshu 
//50-99dianji xingcheng;
//100-149//chengxushiyong
#define JZPingYi_YuanDian  		0x08064000
#define JZPingYi_Shuichi   		0x08064004
#define JZPingYi_YangPin   		0x08064008

#define JZShengJiang_YuanDian 0x0806400c
#define JZShengJiang_YangPin  0x08064010
#define JZShengJiang_ShuiChi  0x08064014

#define CaiYangZhuanPan_1  		0x08064018
#define CaiYangZhuanPan_2  		0x0806401c
#define CaiYangZhuanPan_3  		0x08064020
#define CaiYangZhuanPan_4  		0x08064024
#define CaiYangZhuanPan_5  		0x08064028
#define CaiYangZhuanPan_6  		0x0806402c
#define CaiYangZhuanPan_7  		0x08064030
#define CaiYangZhuanPan_8  		0x08064034
#define CaiYangZhuanPan_9  		0x08064038
#define CaiYangZhuanPan_10 		0x0806403c

#define ChouLvZhuanPan_1  		0x08064040
#define ChouLvZhuanPan_2  		0x08064044
#define ChouLvZhuanPan_3  		0x08064048
#define ChouLvZhuanPan_4  		0x0806404c
#define ChouLvZhuanPan_5  		0x08064050
#define ChouLvZhuanPan_6  		0x08064054
#define ChouLvZhuanPan_7  		0x08064058
#define ChouLvZhuanPan_8  		0x0806405c
#define ChouLvZhuanPan_9  		0x08064060
#define ChouLvZhuanPan_10 		0x08064064

#define CaiYangYaJin_SongKai  0x08064068
#define CaiYangYaJin_YaJin    0x0806406c

#define ChouLvYaJin_SongKai   0x08064070
#define ChouLvYaJin_YaJin     0x08064074

#define JiaZhua_1  					    0x08064078
#define JiaZhua_2  					    0x0806407c
#define JiaZhua_3  					    0x08064080
#define JiaZhua_4  					    0x08064084
#define JiaZhua_5  					    0x08064088
#define JiaZhua_6  					    0x0806408c
#define JiaZhua_7  					    0x08064090
#define JiaZhua_8  					    0x08064094
#define JiaZhua_9  					    0x08064098
#define JiaZhua_10 					    0x0806409c
























extern const uint32_t Flash_Addr[50];

#endif 

