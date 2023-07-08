/**
  ******************************************************************************
  * @file    dht11.c
  * @brief   这个文件提供了DHT11的驱动库
  ******************************************************************************
  */

#include "dht11.h"

/********************************************************
* Function name ：DHT11_Rst
* Description   : 复位DHT11
* Parameter     ：None
* Return        ：None
**********************************************************/
void DHT11_Rst(void)
{
  DHT11_DQ_OUT(0); 	//拉低DQ
  Delay_ms(20);    	//拉低至少18ms
  DHT11_DQ_OUT(1); 	//DQ=1
	Delay_us(40);     	//主机拉高20~40us
}

/********************************************************
* Function name ：DHT11_Check
* Description   : 等待DHT11的回应
* Parameter     ：None
* Return        ：0 -- 存在  ,  1 -- 未检测到DHT11的存在
**********************************************************/
uint8_t DHT11_Check(void) 	   
{
	uint8_t retry=0;
    while (DHT11_DQ_IN()&&retry<100)		//DHT11会拉低40~80us
	{
		retry++;
		Delay_us(1);
	}
	if(retry>=100)return 1;
	else retry=0;
    while (!DHT11_DQ_IN()&&retry<100)		//DHT11拉低后会再次拉高40~80us
	{
		retry++;
		Delay_us(1);
	}
	if(retry>=100)return 1;	    
	return 0;
}

/********************************************************
* Function name ：DHT11_Read_Bit
* Description   : 从DHT11读取一个位
* Parameter     ：None
* Return        ：0 -- fail  ,  1 -- success
**********************************************************/
uint8_t DHT11_Read_Bit(void) 			 
{
 	uint8_t retry=0;
	while(DHT11_DQ_IN()&&retry<100)	//等待变为低电平
	{
		retry++;
		Delay_us(1);
	}
	retry=0;
	while(!DHT11_DQ_IN()&&retry<100)	//等待变高电平
	{
		retry++;
		Delay_us(1);
	}
	Delay_us(40);					//等待40us
	if(DHT11_DQ_IN())return 1;
	else return 0;
}

/********************************************************
* Function name ：DHT11_Read_Byte
* Description   : 从DHT11读取一个字节
* Parameter     ：None
* Return        ：读到的数据
**********************************************************/
uint8_t DHT11_Read_Byte(void)    
{        
    uint8_t i,dat;
    dat=0;
	for (i=0;i<8;i++) 
	{
   		dat<<=1; 
	    dat|=DHT11_Read_Bit();
    }						    
    return dat;
}

/********************************************************
* Function name ：DHT11_Read_Data
* Description   : 从DHT11读取一次数据
* Parameter     ：pbuf：存储读到的数据
* Return        ：0,正常;1,读取失败
**********************************************************/
int8_t DHT11_Read_Data(uint8_t *pbuf)    
{        
 	uint8_t *buf = pbuf;
	uint8_t i;
	DHT11_Rst();
	if(DHT11_Check()==0)
	{
		for(i=0;i<5;i++)	//读取40位数据
		{
			buf[i]=DHT11_Read_Byte();
		}
		if((buf[0]+buf[1]+buf[2]+buf[3])!=buf[4])
		{
			return 2;
		}
	}else 
	{
		return 1;
	}
	return 0;	    
}

