/**
  ******************************************************************************
  * @file    battery.c
  * @brief   这个文件提供了电池电压采集的处理函数
  ******************************************************************************
  */

#include "battery.h"

ADC_HandleTypeDef* BatteryLevel_ADC;
uint16_t adc1_buf[1];

/********************************************************
* Function name ：BatteryLevel_Init
* Description   : 进行采集电压的初始化
* Parameter     ：None
* Return        ：None
**********************************************************/
void BatteryLevel_Init(ADC_HandleTypeDef* hadc)
{
    BatteryLevel_ADC = hadc;
}

/********************************************************
* Function name ：Read_BatteryLevel
* Description   : 采集电压
* Parameter     ：None
* Return        ：None
**********************************************************/
void Read_BatteryLevel()
{
    HAL_ADC_Start_DMA(BatteryLevel_ADC, (uint32_t*)adc1_buf, 1);
}

/********************************************************
* Function name ：Read_BatteryLevel
* Description   : 转换电压
* Parameter     ：None
* Return        ：返回转换后的电压
**********************************************************/
float Return_BatteryLevel()
{
    return ((float)adc1_buf[0]/4096)*3.3;
}

/********************************************************
* Function name ：Read_BatteryLevel
* Description   : 将电压转换为百分比
* Parameter     ：None
* Return        ：None
**********************************************************/
uint8_t VoltTransformToPercentage(float volt)
{
    if(volt >= 4.16){
        return 100;
    } else if(volt >= 4.15 && volt < 4.16){
        return 99;
    } else if(volt >= 4.14 && volt < 4.15){
        return 97;
    } else if(volt >= 4.12 && volt < 4.14){
        return 95;
    } else if(volt >= 4.10 && volt < 4.12){
        return 92;
    } else if(volt >= 4.08 && volt < 4.10){
        return 90;
    } else if(volt >= 4.05 && volt < 4.08){
        return 87;
    } else if(volt >= 4.10 && volt < 4.12){
        return 85;
    } else if(volt >= 4.10 && volt < 4.12){
        return 80;
    } else if(volt >= 4.10 && volt < 4.12){
        return 75;
    } else if(volt >= 4.10 && volt < 4.12){
        return 70;
    } else if(volt >= 4.10 && volt < 4.12){
        return 65;
    } else if(volt >= 4.10 && volt < 4.12){
        return 60;
    } else if(volt >= 4.10 && volt < 4.12){
        return 55;
    } else if(volt >= 4.10 && volt < 4.12){
        return 50;
    } else if(volt >= 4.10 && volt < 4.12){
        return 45;
    } else if(volt >= 4.10 && volt < 4.12){
        return 42;
    } else if(volt >= 4.10 && volt < 4.12){
        return 35;
    } else if(volt >= 4.10 && volt < 4.12){
        return 30;
    } else if(volt >= 4.10 && volt < 4.12){
        return 25;
    } else if(volt >= 4.10 && volt < 4.12){
        return 20;
    } else if(volt >= 4.10 && volt < 4.12){
        return 15;
    } else if(volt >= 4.10 && volt < 4.12){
        return 12;
    } else if(volt >= 4.10 && volt < 4.12){
        return 10;
    } else if(volt >= 4.10 && volt < 4.12){
        return 8;
    } else if(volt >= 4.10 && volt < 4.12){
        return 5;
    } else if(volt >= 4.10 && volt < 4.12){
        return 3;
    } else if(volt >= 4.10 && volt < 4.12){
        return 1;
    } else{
        return 50;
    }
}


