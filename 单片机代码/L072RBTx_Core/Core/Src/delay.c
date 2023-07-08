/**
  ******************************************************************************
  * @file    delay.c
  * @brief   ����ļ��ṩ����ʱ����
  ******************************************************************************
  */

#include "delay.h"

/**********************************************************
��  �ܣ�΢�뼶��ʱ
��  ����n ΢�뱶�������� n = 1 �� ��ʱ1us
����ֵ����
**********************************************************/
//1us
void Delay_us(uint32_t n)
{

	SysTick->CTRL = 0; // Disable SysTick������ SysTick
	SysTick->LOAD = (32*n)-1; // ���ü���ֵ��32000000/1000000��-1 ��0
	SysTick->VAL = 0; // Clear current value as well as count flag�������ǰֵ�Լ�������־
	SysTick->CTRL = 5; // Enable SysTick timer with processor clock��ʹ�ô�����ʱ������ SysTick ��ʱ��
	while ((SysTick->CTRL & 0x00010000)==0);// Wait until count flag is set

	SysTick->CTRL = 0; // Disable SysTick
}

/**********************************************************
��  �ܣ����뼶��ʱ
��  ����n ΢�뱶�������� n = 1 �� ��ʱ1ms
����ֵ����
**********************************************************/
//1ms
void Delay_ms(uint32_t n)
{
	
	while(n--)
	{
		SysTick->CTRL = 0; // Disable SysTick������ SysTick
		SysTick->LOAD = (32000000/1000)-1; // ���ü���ֵ��168000000/1000��-1 ��0
		SysTick->VAL = 0; // Clear current value as well as count flag�������ǰֵ�Լ�������־
		SysTick->CTRL = 5; // Enable SysTick timer with processor clock��ʹ�ô�����ʱ������ SysTick ��ʱ��
		while ((SysTick->CTRL & 0x00010000)==0);// Wait until count flag is set

	}
	SysTick->CTRL = 0; // Disable SysTick
}
