/********************************************************************************
* @File name: key.h
* @Author: fufeng
* @Version: 1.0
* @Date: 2023-6-22
* @brief   This file contains all the function prototypes for the key.c file
********************************************************************************/
#ifndef __KEY_H__
#define __KEY_H__

#include "main.h"
#define KEY0() 		HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin)
#define KEY1() 		HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin)

uint8_t Key_Scan(uint8_t);  		//°´¼üÉ¨Ãèº¯Êý

#endif //__KEY_H__
