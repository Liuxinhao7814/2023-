/**
  ******************************************************************************
  * @file    dht11.c
  * @brief   ����ļ��ṩ��DHT11��������
  ******************************************************************************
  */

#include "dht11.h"

/********************************************************
* Function name ��DHT11_Rst
* Description   : ��λDHT11
* Parameter     ��None
* Return        ��None
**********************************************************/
void DHT11_Rst(void)
{
  DHT11_DQ_OUT(0); 	//����DQ
  Delay_ms(20);    	//��������18ms
  DHT11_DQ_OUT(1); 	//DQ=1
	Delay_us(40);     	//��������20~40us
}

/********************************************************
* Function name ��DHT11_Check
* Description   : �ȴ�DHT11�Ļ�Ӧ
* Parameter     ��None
* Return        ��0 -- ����  ,  1 -- δ��⵽DHT11�Ĵ���
**********************************************************/
uint8_t DHT11_Check(void) 	   
{
	uint8_t retry=0;
    while (DHT11_DQ_IN()&&retry<100)		//DHT11������40~80us
	{
		retry++;
		Delay_us(1);
	}
	if(retry>=100)return 1;
	else retry=0;
    while (!DHT11_DQ_IN()&&retry<100)		//DHT11���ͺ���ٴ�����40~80us
	{
		retry++;
		Delay_us(1);
	}
	if(retry>=100)return 1;	    
	return 0;
}

/********************************************************
* Function name ��DHT11_Read_Bit
* Description   : ��DHT11��ȡһ��λ
* Parameter     ��None
* Return        ��0 -- fail  ,  1 -- success
**********************************************************/
uint8_t DHT11_Read_Bit(void) 			 
{
 	uint8_t retry=0;
	while(DHT11_DQ_IN()&&retry<100)	//�ȴ���Ϊ�͵�ƽ
	{
		retry++;
		Delay_us(1);
	}
	retry=0;
	while(!DHT11_DQ_IN()&&retry<100)	//�ȴ���ߵ�ƽ
	{
		retry++;
		Delay_us(1);
	}
	Delay_us(40);					//�ȴ�40us
	if(DHT11_DQ_IN())return 1;
	else return 0;
}

/********************************************************
* Function name ��DHT11_Read_Byte
* Description   : ��DHT11��ȡһ���ֽ�
* Parameter     ��None
* Return        ������������
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
* Function name ��DHT11_Read_Data
* Description   : ��DHT11��ȡһ������
* Parameter     ��pbuf���洢����������
* Return        ��0,����;1,��ȡʧ��
**********************************************************/
int8_t DHT11_Read_Data(uint8_t *pbuf)    
{        
 	uint8_t *buf = pbuf;
	uint8_t i;
	DHT11_Rst();
	if(DHT11_Check()==0)
	{
		for(i=0;i<5;i++)	//��ȡ40λ����
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

