/********************************************************************************
* @File name: GY30.h
* @Author: fufeng
* @Version: 1.0
* @Date: 2023-6-22
* @brief   This file contains all the function prototypes for the GY30.c file
********************************************************************************/
#ifndef __GY30_H__
#define __GY30_H__

#include "main.h"

#define GY30_Read_Addr  0x47  //SGP30的读地址
#define GY30_Write_Addr 0x46  //SGP30的写地址

void GY30_Init(I2C_HandleTypeDef* hi2c);
void GY30_Read();
uint32_t GY30_ReturnLight();

#endif