/**
  ******************************************************************************
  * @file    delay.c
  * @brief   这个文件提供了延时函数
  ******************************************************************************
  */

#include "delay.h"

/**********************************************************
功  能：微秒级延时
参  数：n 微秒倍数、例如 n = 1 则 延时1us
返回值：无
**********************************************************/
//1us
void Delay_us(uint32_t n)
{

	SysTick->CTRL = 0; // Disable SysTick　禁用 SysTick
	SysTick->LOAD = (32*n)-1; // 配置计数值（32000000/1000000）-1 到0
	SysTick->VAL = 0; // Clear current value as well as count flag　清除当前值以及计数标志
	SysTick->CTRL = 5; // Enable SysTick timer with processor clock　使用处理器时钟启用 SysTick 计时器
	while ((SysTick->CTRL & 0x00010000)==0);// Wait until count flag is set

	SysTick->CTRL = 0; // Disable SysTick
}

/**********************************************************
功  能：毫秒级延时
参  数：n 微秒倍数、例如 n = 1 则 延时1ms
返回值：无
**********************************************************/
//1ms
void Delay_ms(uint32_t n)
{
	
	while(n--)
	{
		SysTick->CTRL = 0; // Disable SysTick　禁用 SysTick
		SysTick->LOAD = (32000000/1000)-1; // 配置计数值（168000000/1000）-1 到0
		SysTick->VAL = 0; // Clear current value as well as count flag　清除当前值以及计数标志
		SysTick->CTRL = 5; // Enable SysTick timer with processor clock　使用处理器时钟启用 SysTick 计时器
		while ((SysTick->CTRL & 0x00010000)==0);// Wait until count flag is set

	}
	SysTick->CTRL = 0; // Disable SysTick
}
