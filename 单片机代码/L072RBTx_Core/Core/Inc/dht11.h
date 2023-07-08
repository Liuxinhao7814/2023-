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

//IO��������
#define	DHT11_DQ_OUT(X) HAL_GPIO_WritePin(DHT11_GPIO_Port,DHT11_Pin,(GPIO_PinState)X)
#define	DHT11_DQ_IN()  	HAL_GPIO_ReadPin(DHT11_GPIO_Port,DHT11_Pin)


void    DHT11_Init();
int8_t  DHT11_Read_Data(uint8_t *pbuf);	//��ȡ��ʪ��
uint8_t DHT11_Read_Byte(void);				//����һ���ֽ�
uint8_t DHT11_Read_Bit(void);				//����һ��λ
uint8_t DHT11_Check(void);					//����Ƿ����DHT11
void DHT11_Rst(void);						//��λDHT11   

	

#endif
 
