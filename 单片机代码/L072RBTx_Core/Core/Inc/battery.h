/********************************************************************************
* @File name: battery.h
* @Author: fufeng
* @Version: 1.0
* @Date: 2023-6-22
* @brief   This file contains all the function prototypes for the battery.c file
********************************************************************************/

#ifndef __BATTERY_H__
#define __BATTERY_H__
#include "main.h"

#define PARTIAL_PRESSURE_COEFFICIENT 2.9432

void    BatteryLevel_Init(ADC_HandleTypeDef* hadc);
void    Read_BatteryLevel();
float   Return_BatteryLevel();
uint8_t VoltTransformToPercentage(float volt);


#endif //L072RBTX_CORE_BATTERY_H
