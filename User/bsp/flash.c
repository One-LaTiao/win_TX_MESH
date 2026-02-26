#include "flash.h"



extern void    FLASH_PageErase(uint32_t PageAddress); 



const uint32_t Flash_Addr[50]={
	0x08064000,//[0]JZPingYi_YuanDian  		
	0x08064004,//[1]JZPingYi_Shuichi   		
	0x08064008,//[2]JZPingYi_YangPin   		
	0x0806400c,//[3]JZShengJiang_YuanDian 
	0x08064010,//[4]JZShengJiang_YangPin  
	0x08064014,//[5]JZShengJiang_ShuiChi  
	0x08064018,//[6]CaiYangZhuanPan_1  			
	0x0806401c,//[7]CaiYangZhuanPan_2  			
	0x08064020,//[8]CaiYangZhuanPan_3  			
	0x08064024,//[9]CaiYangZhuanPan_4  			
	0x08064028,//[10]CaiYangZhuanPan_5  			
	0x0806402c,//[11]CaiYangZhuanPan_6  			
	0x08064030,//[12]CaiYangZhuanPan_7  			
	0x08064034,//[13]CaiYangZhuanPan_8  			
	0x08064038,//[14]CaiYangZhuanPan_9  			
	0x0806403c,//[15]CaiYangZhuanPan_10 				       
	0x08064040,//[16]ChouLvZhuanPan_1  			
	0x08064044,//[17]ChouLvZhuanPan_2  			
	0x08064048,//[18]ChouLvZhuanPan_3  			
	0x0806404c,//[19]ChouLvZhuanPan_4  			
	0x08064050,//[20]ChouLvZhuanPan_5  			
	0x08064054,//[21]ChouLvZhuanPan_6  			
	0x08064058,//[22]ChouLvZhuanPan_7  			
	0x0806405c,//[23]ChouLvZhuanPan_8  			
	0x08064060,//[24]ChouLvZhuanPan_9  			
	0x08064064,//[25]ChouLvZhuanPan_10 				       
	0x08064068,//[26]CaiYangYaJin_SongKai  	
	0x0806406c,//[27]CaiYangYaJin_YaJin    	       
	0x08064070,//[28]ChouLvYaJin_SongKai   	
	0x08064074,//[29]ChouLvYaJin_YaJin     	
	0x08064078,//[30]夹爪1号位
	0x0806407c,//[31]夹爪2号位
	0x08064080,//[32]夹爪3号位
	0x08064084,//[33]夹爪4号位
	0x08064088,//[34]夹爪5号位
	0x0806408c,//[35]夹爪6号位
	0x08064090,//[36]夹爪7号位
	0x08064094,//[37]夹爪8号位
	0x08064098,//[38]夹爪9号位
	0x0806409c,//[39]夹爪10号位
	0x080640A0,//[40]超声纯水蠕动泵速度
	0x080640A4,//[41]称重纯水蠕动泵速度
	0x080640A8,//[42]样品排出蠕动泵速度
	0x080640Ac,//[43]搅拌电机速度
};

 
//读取指定地址的半字(16位数据) 
//faddr:读地址 
//返回值:对应数据.
uint32_t STMFLASH_ReadHalfWord(uint32_t faddr)
{
	return *(uint32_t*)faddr; 
}


#if STM32_FLASH_WREN	//如果使能了写   
//不检查的写入
//WriteAddr:起始地址
//pBuffer:数据指针
//NumToWrite:半字(16位)数   
void STMFLASH_Write_NoCheck(uint32_t WriteAddr,const uint16_t *pBuffer,uint16_t NumToWrite)   
{ 			 		 
	uint16_t i;
	for(i=0;i<NumToWrite;i++)
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,WriteAddr,pBuffer[i]);
	    WriteAddr+=2;//地址增加2.
	}  
} 
//从指定地址开始写入指定长度的数据
//WriteAddr:起始地址(此地址必须为2的倍数!!)
//pBuffer:数据指针
//NumToWrite:半字(16位)数(就是要写入的16位数据的个数.)
#if STM32_FLASH_SIZE<256
#define STM_SECTOR_SIZE 1024 //字节
#else 
#define STM_SECTOR_SIZE	2048
#endif		 
uint16_t STMFLASH_BUF[STM_SECTOR_SIZE/2];//最多是2K字节
void STMFLASH_Write(uint32_t WriteAddr,const uint16_t *pBuffer,uint16_t NumToWrite)	
{
	uint32_t secpos;	   //扇区地址
	uint16_t secoff;	   //扇区内偏移地址(16位字计算)
	uint16_t secremain; //扇区内剩余地址(16位字计算)	   
 	uint16_t i;    
	uint32_t offaddr;   //去掉0X08000000后的地址
	
	if(WriteAddr<STM32_FLASH_BASE||(WriteAddr>=(STM32_FLASH_BASE+1024*STM32_FLASH_SIZE)))return;//非法地址
	
	HAL_FLASH_Unlock();					//解锁
	offaddr=WriteAddr-STM32_FLASH_BASE;		//实际偏移地址.
	secpos=offaddr/STM_SECTOR_SIZE;			//扇区地址  0~127 for STM32F103RBT6
	secoff=(offaddr%STM_SECTOR_SIZE)/2;		//在扇区内的偏移(2个字节为基本单位.)
	secremain=STM_SECTOR_SIZE/2-secoff;		//扇区剩余空间大小   
	if(NumToWrite<=secremain)secremain=NumToWrite;//不大于该扇区范围
	while(1) 
	{	
		STMFLASH_Read(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//读出整个扇区的内容
		for(i=0;i<secremain;i++)	//校验数据
		{
			if(STMFLASH_BUF[secoff+i]!=0XFFFF)break;//需要擦除  	  
		}
		if(i<secremain)				//需要擦除
		{
			FLASH_PageErase(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE);	//擦除这个扇区
			FLASH_WaitForLastOperation(FLASH_WAITETIME);            	//等待上次操作完成
			CLEAR_BIT(FLASH->CR, FLASH_CR_PER);							//清除CR寄存器的PER位，此操作应该在FLASH_PageErase()中完成！
																		//但是HAL库里面并没有做，应该是HAL库bug！
			for(i=0;i<secremain;i++)//复制
			{
				STMFLASH_BUF[i+secoff]=pBuffer[i];	  
			}
			STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//写入整个扇区  

		}
		else 
		{
			FLASH_WaitForLastOperation(FLASH_WAITETIME);       	//等待上次操作完成
			STMFLASH_Write_NoCheck(WriteAddr,pBuffer,secremain);//写已经擦除了的,直接写入扇区剩余区间. 
		}

		if(NumToWrite==secremain)
			break;//写入结束了
		else//写入未结束
		{
			secpos++;				//扇区地址增1
			secoff=0;				//偏移位置为0 	 
		   	pBuffer+=secremain;  	//指针偏移
			WriteAddr+=secremain*2;	//写地址偏移(16位数据地址,需要*2)	   
		   	NumToWrite-=secremain;	//字节(16位)数递减
			if(NumToWrite>(STM_SECTOR_SIZE/2))secremain=STM_SECTOR_SIZE/2;//下一个扇区还是写不完
			else secremain=NumToWrite;//下一个扇区可以写完了
		}	 
	};	
	HAL_FLASH_Lock();		//上锁
}
#endif

//从指定地址开始读出指定长度的数据
//ReadAddr:起始地址
//pBuffer:数据指针
//NumToWrite:半字(16位)数
void STMFLASH_Read(uint32_t ReadAddr,uint16_t *pBuffer,uint16_t NumToRead)   	
{
	uint16_t i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadHalfWord(ReadAddr);//读取2个字节.
		ReadAddr+=2;//偏移2个字节.	
	}
}










