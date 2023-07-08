/**
  ******************************************************************************
  * @file    GY30.c
  * @brief   这个文件提供了GY-30光照强度传感器的驱动库
  ******************************************************************************
  */

#include "GY30.h"

I2C_HandleTypeDef* GY30_I2C;
uint8_t GY30_Receive[3];
uint8_t GY30_Accu_Reg = 0x10;

/********************************************************
* Function name ：GY30_Init
* Description   : 初始化GY30
* Parameter     ：None
* Return        ：None
**********************************************************/
void GY30_Init(I2C_HandleTypeDef* hi2c)
{
	GY30_I2C = hi2c;
	HAL_I2C_Master_Transmit(GY30_I2C,GY30_Write_Addr,&GY30_Accu_Reg,1,10);
	Delay_ms(10);
}

/********************************************************
* Function name ：GY30_Read
* Description   : 读取GY30的传感器值
* Parameter     ：None
* Return        ：None
**********************************************************/
void GY30_Read(){
    HAL_I2C_Master_Receive(GY30_I2C,GY30_Read_Addr,GY30_Receive,3,10);
}

/********************************************************
* Function name ：GY30_ReturnLight
* Description   : 转换传感数据
* Parameter     ：None
* Return        ：转换后的值
**********************************************************/
uint32_t GY30_ReturnLight()
{
    uint32_t dat;
    dat = GY30_Receive[0];
    dat <<= 8;
    dat |= GY30_Receive[1];
    dat /= 1.2;
    return dat;
}