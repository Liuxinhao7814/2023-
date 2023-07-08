/********************************************************************************
* @File name: dht11.h
* @Author: fufeng
* @Version: 1.0
* @Date: 2023-6-22
* @brief   This file contains all the function prototypes for the dht11.c file
********************************************************************************/
#ifndef __DHT11_H__
#define __DHT11_H__

#include "main.h"

//IO操作函数
#define	DHT11_DQ_OUT(X) HAL_GPIO_WritePin(DHT11_GPIO_Port,DHT11_Pin,(GPIO_PinState)X)
#define	DHT11_DQ_IN()  	HAL_GPIO_ReadPin(DHT11_GPIO_Port,DHT11_Pin)


void    DHT11_Init();
int8_t  DHT11_Read_Data(uint8_t *pbuf);	//读取温湿度
uint8_t DHT11_Read_Byte(void);				//读出一个字节
uint8_t DHT11_Read_Bit(void);				//读出一个位
uint8_t DHT11_Check(void);					//检测是否存在DHT11
void DHT11_Rst(void);						//复位DHT11   

	

#endif
 
