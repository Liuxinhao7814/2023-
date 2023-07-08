/**
  ******************************************************************************
  * @file    zigbee.c
  * @brief   这个文件提供了E18 ZigBee3.0的库
  ******************************************************************************
  */

#include "zigbee.h"
#include "dht11.h"
#include "GY30.h"
#include "battery.h"
#include "sensor_type.h"


uint8_t E18_RECEIVE_BUFF[100];
uint8_t RX_NUM = 0;
uint8_t RX_NUM_LENGTH = 0;
uint8_t E18_CMD_FLAG = 0;//等于0时表示E18ZigBee配置/读取参数结束,等于1时表示当前在配置/读取参数
E18_STATUS ZigBee;
extern unsigned char UART4_temp[1];
uint8_t  E18_Send_DataBuf[30] = {0};
bool E18_NodeControlCMD;
uint8_t UDP_HEAD[4] = {0x4B,0x4B,0x50,0x50};  // 包头
uint8_t UDP_TAIL[4] = {0x50,0x50,0x4B,0x4B}; // 包尾

/********************************************************
* Function name ：E18_HARDWARE_Init
* Description   : 初始化ZigBee
* Parameter     ：None
* Return        ：None
**********************************************************/
void E18_HARDWARE_Init()
{
	/**********组播********/
    E18_NodeControlCMD = 1;
	while(!E18_Read_SerialPortMode());
	Delay_ms(100);
	E18_Set_CommandMode();
	Delay_ms(1000);
	while(!E18_Read_InitialState());
	Delay_ms(100);
	while(!E18_Read_status());
	Delay_ms(100);
	if(ZigBee.E18_Network_Status == 0x00 && ZigBee.E18_PANID[0] == E18_PANID_0 && ZigBee.E18_PANID[1] == E18_PANID_1)
	{
		while(!E18_Set_TransparentMode());
	}else{
	    while(!E18_Recovery());
		Delay_ms(2000);
		while(!E18_Set_DevType(E18_Terminal));
		Delay_ms(100);
		while(!E18_Set_Power(E18_POWER_18));
		Delay_ms(100);
		while(!E18_Set_LowPower(E18_CYCLE_1S));
		Delay_ms(100);
		while(!E18_Reset());
		Delay_ms(2000);
		while(!E18_Open_Network());
		Delay_ms(5000);
		while(!E18_Read_InitialState());
		Delay_ms(100);
		while(!E18_Set_Group(0x11, 0x22));
		Delay_ms(100);
		while(!E18_Set_Target_ShortAddress(0x11, 0x22));
		Delay_ms(100);
		while(!E18_Set_Target_Port(0xFF));
		Delay_ms(100);
	}
}

/********************************************************
* Function name ：E18_Open_Network
* Description   : 向ZigBee发送打开网络的指令
* Parameter     ：None
* Return        ：0 -- fail  ,  1 -- success
**********************************************************/
uint8_t E18_Open_Network(void)
{
	uint16_t tc = 0;
	uint8_t dat[5] = {0x55, 0x03, 0x00, 0x02, 0x02};

	if(ZigBee.E18_Network_Status)
	{
		ZigBee_Send_Data_length(dat, 5);
	}else{
		return 1;
	}
	
	RX_NUM = 0;
	RX_NUM_LENGTH = 5 - 1;
	E18_CMD_FLAG = 1;

	HAL_UART_Receive_IT(E18_USART,(uint8_t *)UART4_temp, 1);
	while(E18_CMD_FLAG)
	{
		Delay_ms(1);
		tc++;
		if(tc > 2000)
		{
			printf("E18_Reset TimeOut\n\r");
			return 0;
		}
	}
	
	E18_RECEIVE_BUFF[0] = 0;
	if(E18_RECEIVE_BUFF[4] == 0x00)
	{
		return 1;
	}
	return 0;
}

/********************************************************
* Function name ：E18_Close_Network
* Description   : 向ZigBee发送关闭网络的指令
* Parameter     ：None
* Return        ：0 -- fail  ,  1 -- success
**********************************************************/
uint8_t 	E18_Close_Network(void)
{
	uint16_t tc = 0;
	uint8_t dat[5] = {0x55, 0x03, 0x00, 0x03, 0x03};

	RX_NUM = 0;
	RX_NUM_LENGTH = 5 - 1;
	E18_CMD_FLAG = 1;

	if(!ZigBee.E18_Network_Status)
	{
		ZigBee_Send_Data_length(dat, 5);
	}else{
		return 1;
	}

	HAL_UART_Receive_IT(E18_USART,(uint8_t *)UART4_temp, 1);
	while(E18_CMD_FLAG)
	{
		Delay_ms(1);
		tc++;
		if(tc > 2000)
		{
			return 0;
		}
	}
	
	E18_RECEIVE_BUFF[0] = 0;
	if(E18_RECEIVE_BUFF[4] == 0x00)
	{
		return 1;
	}
	return 0;
}

/********************************************************
* Function name ：E18_Reset
* Description   : 向ZigBee发送复位的指令
* Parameter     ：None
* Return        ：0 -- fail  ,  1 -- success
**********************************************************/
uint8_t 	E18_Reset(void)
{
	uint16_t tc = 0;
	uint8_t dat[9] = {0x55, 0x07, 0x00, 0x04, 0x00, 0xFF, 0xFF, 0x00, 0xEF};
	dat[8] = E18_Generate_CheckCode(6, dat);

	RX_NUM = 0;
	RX_NUM_LENGTH = 7 - 1;
	E18_CMD_FLAG = 1;

	ZigBee_Send_Data_length(dat, 9);

	HAL_UART_Receive_IT(E18_USART,(uint8_t *)UART4_temp, 1);
	while(E18_CMD_FLAG)
	{
		Delay_ms(1);
		tc++;
		if(tc > 2000)
		{
			printf("E18_Reset TimeOut\n\r");
			return 0;
		}
	}
	
	E18_RECEIVE_BUFF[0] = 0;
	if(E18_RECEIVE_BUFF[3] == 0x00)
	{
		return 1;
	}
	return 0;
}

/********************************************************
* Function name ：E18_Leave
* Description   : 向ZigBee发送退出网络的指令
* Parameter     ：None
* Return        ：0 -- fail  ,  1 -- success
**********************************************************/
uint8_t 	E18_Leave(void)
{
	E18_CHANNEL channel;
	uint16_t tc = 0;
	uint8_t dat[9] = {0x55, 0x07, 0x00, 0x04, 0x01, 0xFF, 0xFF, 0x00, 0xEE};
	while(!E18_Read_PanID());
	dat[5] = ZigBee.E18_PANID[0];
	dat[6] = ZigBee.E18_PANID[1];
	while(!E18_Read_Channel);
	dat[7] = ZigBee.E18_Channel;
	dat[8] = E18_Generate_CheckCode(6, dat);

	RX_NUM = 0;
	RX_NUM_LENGTH = 7 - 1;
	E18_CMD_FLAG = 1;

	if(!ZigBee.E18_Network_Status)
	{
		ZigBee_Send_Data_length(dat, 9);
	}else{
		return 1;
	}

	HAL_UART_Receive_IT(E18_USART,(uint8_t *)UART4_temp, 1);
	while(E18_CMD_FLAG)
	{
		Delay_ms(1);
		tc++;
		if(tc > 2000)
		{
			return 0;
		}
	}
	
	E18_RECEIVE_BUFF[0] = 0;
	if(E18_RECEIVE_BUFF[3] == 0x00)
	{
		Delay_ms(100);
		while(!E18_Read_InitialState());
		return 1;
	}
	return 0;
}

/********************************************************
* Function name ：E18_Recovery
* Description   : 向ZigBee发送恢复出厂设置的指令
* Parameter     ：None
* Return        ：0 -- fail  ,  1 -- success
**********************************************************/
uint8_t 	E18_Recovery(void)
{
	E18_CHANNEL channel;
	uint16_t tc = 0;
	uint8_t dat[9] = {0x55, 0x07, 0x00, 0x04, 0x02, 0xFF, 0xFF, 0x00, 0xED};

	if(ZigBee.E18_Network_Status)
	{
		return 1;
	}

	dat[5] = ZigBee.E18_PANID[0];
	dat[6] = ZigBee.E18_PANID[1];
	dat[7] = ZigBee.E18_Channel;
	dat[8] = E18_Generate_CheckCode(6, dat);

	RX_NUM = 0;
	RX_NUM_LENGTH = 7 - 1;
	E18_CMD_FLAG = 1;

	ZigBee_Send_Data_length(dat, 9);

	HAL_UART_Receive_IT(E18_USART,(uint8_t *)UART4_temp, 1);
	while(E18_CMD_FLAG)
	{
		Delay_ms(1);
		tc++;
		if(tc > 2000)
		{
			return 0;
		}
	}
	
	E18_RECEIVE_BUFF[0] = 0;
	if(E18_RECEIVE_BUFF[3] == 0x00)
	{
		Delay_ms(100);
		while(!E18_Read_InitialState());
		return 1;
	}
	return 0;
}

/********************************************************
* Function name ：E18_Read_InitialState
* Description   : 发送命令,获取ZigBee当前的组网状态和设备类型
* Parameter     ：None
* Return        ：0 -- fail  ,  1 -- success
**********************************************************/
uint8_t		E18_Read_InitialState(void)
{
	uint16_t tc = 0;
	uint8_t dat[5] = {0x55, 0x03, 0x00, 0x00, 0x00};

	RX_NUM = 0;
	RX_NUM_LENGTH = 15 - 1;
	E18_CMD_FLAG = 1;

	ZigBee_Send_Data_length(dat, 5);

	HAL_UART_Receive_IT(E18_USART,(uint8_t *)UART4_temp, 1);
	while(E18_CMD_FLAG)
	{
		Delay_ms(1);
		tc++;
		if(tc > 2000)
		{
			printf("E18_Read_InitialState TimeOut\n\r");
			return 0;
		}
	}
	
	E18_RECEIVE_BUFF[0] = 0;
	if(E18_RECEIVE_BUFF[4] == 0xFF)		//未组网
	{
		ZigBee.E18_Network_Status = 1;
		ZigBee.E18_DevType = E18_RECEIVE_BUFF[5];
		return 1;
	}else if(E18_RECEIVE_BUFF[4] == 0x00){	//已组网
		ZigBee.E18_Network_Status = 0;
		ZigBee.E18_DevType = E18_RECEIVE_BUFF[5];
		return 1;
	}
	return 0;
}

/********************************************************
* Function name ：E18_Read_status
* Description   : 发送命令,获取ZigBee当前的组网状态、设备类型、
*                  MAC地址、扩展ID、信道、PANID、短地址、网络密钥
* Parameter     ：None
* Return        ：0 -- fail  ,  1 -- success
**********************************************************/
uint8_t 	E18_Read_status(void)
{
	uint16_t tc = 0;
	uint8_t i;
	uint8_t dat[5] = {0x55, 0x03, 0x00, 0x00, 0x00};

	if(!ZigBee.E18_Network_Status)
	{
		RX_NUM_LENGTH = 44 - 1;
	}else {
		RX_NUM_LENGTH = 15 - 1;
	}

	RX_NUM = 0;
	E18_CMD_FLAG = 1;
	ZigBee_Send_Data_length(dat, 5);

	HAL_UART_Receive_IT(E18_USART,(uint8_t *)UART4_temp, 1);
	while(E18_CMD_FLAG)
	{
		Delay_ms(1);
		tc++;
		if(tc > 2000)
		{
			printf("E18_Read_status TimeOut\n\r");
			return 0;
		}
	}
	
	E18_RECEIVE_BUFF[0] = 0;
	if(E18_RECEIVE_BUFF[4] == 0xFF)		//未组网
	{
		ZigBee.E18_Network_Status = 1;
		ZigBee.E18_DevType = E18_RECEIVE_BUFF[5];
		for(i = 0; i < 8; i++)
		{
			ZigBee.E18_MAC[i] = E18_RECEIVE_BUFF[6 + i];
		}
		return 1;
	}else if(E18_RECEIVE_BUFF[4] == 0x00){	//已组网
		ZigBee.E18_Network_Status = 0;
		ZigBee.E18_DevType = E18_RECEIVE_BUFF[5];
		for(i = 0; i < 8; i++)
		{
			ZigBee.E18_MAC[i] = E18_RECEIVE_BUFF[6 + i];
			ZigBee.E18_Extension_ID[i] = E18_RECEIVE_BUFF[19 + i];
		}
		ZigBee.E18_Channel 	= E18_RECEIVE_BUFF[14];
		ZigBee.E18_PANID[0] = E18_RECEIVE_BUFF[15];
		ZigBee.E18_PANID[1] = E18_RECEIVE_BUFF[16];
		ZigBee.E18_ShortAddress[0] = E18_RECEIVE_BUFF[17];
		ZigBee.E18_ShortAddress[1] = E18_RECEIVE_BUFF[18];
		for(i = 0; i < 16; i++)
		{
			ZigBee.E18_SecretKey[i] = E18_RECEIVE_BUFF[27 + i];
		}
		return 1;
	}
	return 0;
}

/********************************************************
* Function name ：E18_Read_Channel
* Description   : 发送命令,获取ZigBee当前的信道
* Parameter     ：None
* Return        ：0 -- fail  ,  1 -- success
**********************************************************/
uint8_t E18_Read_Channel(void)
{
	uint16_t tc = 0;
	uint8_t dat[6] = {0x55, 0x04, 0x00, 0x06, 0x00, 0x06};

	RX_NUM = 0;
	RX_NUM_LENGTH = 7 - 1;
	E18_CMD_FLAG = 1;

	if(ZigBee.E18_Network_Status)
	{
		ZigBee_Send_Data_length(dat, 6);
	}else{
		return 1;
	}

	HAL_UART_Receive_IT(E18_USART,(uint8_t *)UART4_temp, 1);
	while(E18_CMD_FLAG)
	{
		Delay_ms(1);
		tc++;
		if(tc > 2000)
		{
			return 0;
		}
	}
	
	E18_RECEIVE_BUFF[0] = 0;
	if(E18_RECEIVE_BUFF[4] == 0x00)
	{
		ZigBee.E18_Channel = E18_RECEIVE_BUFF[5];
		return 1;
	}
	return 0;
}

/********************************************************
* Function name ：E18_Read_PanID
* Description   : 发送命令,获取ZigBee当前的PANID
* Parameter     ：None
* Return        ：0 -- fail  ,  1 -- success
**********************************************************/
uint8_t 	E18_Read_PanID(void)
{
	uint16_t tc = 0;
	uint8_t dat[5] = {0x55, 0x03, 0x00, 0x07, 0x07};

	RX_NUM = 0;
	RX_NUM_LENGTH = 8 - 1;
	E18_CMD_FLAG = 1;

	if(!ZigBee.E18_Network_Status)
	{
		ZigBee_Send_Data_length(dat, 6);
	}else{
		return 1;
	}

	HAL_UART_Receive_IT(E18_USART,(uint8_t *)UART4_temp, 1);
	while(E18_CMD_FLAG)
	{
		Delay_ms(1);
		tc++;
		if(tc > 2000)
		{
			return 0;
		}
	}
	
	E18_RECEIVE_BUFF[0] = 0;
	if(E18_RECEIVE_BUFF[4] == 0x00)
	{
		ZigBee.E18_PANID[0] = E18_RECEIVE_BUFF[5];
		ZigBee.E18_PANID[1] = E18_RECEIVE_BUFF[6];
		return 1;
	}
	return 0;
}

/********************************************************
* Function name ：E18_Read_Group
* Description   : 发送命令,获取ZigBee当前所加的组号
* Parameter     ：None
* Return        ：0 -- fail  ,  1 -- success
**********************************************************/
uint8_t 	E18_Read_Group(void)
{
	uint16_t tc = 0;
	uint8_t	i;
	uint8_t dat[6] = {0x55, 0x04, 0x00, 0x09, 0x00, 0x09};

	RX_NUM = 0;
	RX_NUM_LENGTH = 9 - 1;
	E18_CMD_FLAG = 1;

	ZigBee_Send_Data_length(dat, 6);

	HAL_UART_Receive_IT(E18_USART,(uint8_t *)UART4_temp, 1);
	while(E18_CMD_FLAG)
	{
		Delay_ms(1);
		tc++;
		if(tc > 2000)
		{
			return 0;
		}
	}
	
	E18_RECEIVE_BUFF[0] = 0;
	if(E18_RECEIVE_BUFF[4] == 0x00)
	{
		ZigBee.E18_Group[0] = E18_RECEIVE_BUFF[6];
		ZigBee.E18_Group[1] = E18_RECEIVE_BUFF[7];
		return 1;
	}
	return 0;
}

/********************************************************
* Function name ：E18_Read_Power
* Description   : 发送命令,获取ZigBee当前的发射功率
* Parameter     ：None
* Return        ：0 -- fail  ,  1 -- success
**********************************************************/
uint8_t 	E18_Read_Power(void)
{
	uint16_t tc = 0;
	uint8_t dat[6] = {0x55, 0x04, 0x00, 0x0D, 0x00, 0x0D};

	RX_NUM = 0;
	RX_NUM_LENGTH = 7 - 1;
	E18_CMD_FLAG = 1;

	ZigBee_Send_Data_length(dat, 6);

	HAL_UART_Receive_IT(E18_USART,(uint8_t *)UART4_temp, 1);
	while(E18_CMD_FLAG)
	{
		Delay_ms(1);
		tc++;
		if(tc > 2000)
		{
			return 0;
		}
	}
	
	E18_RECEIVE_BUFF[0] = 0;
	if(E18_RECEIVE_BUFF[4] == 0x00)
	{
		ZigBee.E18_Power_Grade = E18_RECEIVE_BUFF[5];
		return 1;
	}
	return 0;
}

/********************************************************
* Function name ：E18_Read_Baud
* Description   : 发送命令,获取ZigBee当前的串口波特率
* Parameter     ：None
* Return        ：0 -- fail  ,  1 -- success
**********************************************************/
uint8_t 	E18_Read_Baud(void)
{
	uint16_t tc = 0;
	uint8_t dat[8] = {0x55, 0x06, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10};

	RX_NUM = 0;
	RX_NUM_LENGTH = 10 - 1;
	E18_CMD_FLAG = 1;

	ZigBee_Send_Data_length(dat, 8);

	HAL_UART_Receive_IT(E18_USART,(uint8_t *)UART4_temp, 1);
	while(E18_CMD_FLAG)
	{
		Delay_ms(1);
		tc++;
		if(tc > 2000)
		{
			return 0;
		}
	}
	
	E18_RECEIVE_BUFF[0] = 0;
	if(E18_RECEIVE_BUFF[4] == 0x00)
	{
		if(E18_RECEIVE_BUFF[5] == 0x80 && E18_RECEIVE_BUFF[6] == 0x25)
		{
			ZigBee.E18_Baud = E18_Baud_9600;
			return 1;
		}else if(E18_RECEIVE_BUFF[5] == 0x00 && E18_RECEIVE_BUFF[6] == 0x4B)
		{
			ZigBee.E18_Baud = E18_Baud_19200;
			return 1;
		}else if(E18_RECEIVE_BUFF[5] == 0x00 && E18_RECEIVE_BUFF[6] == 0x96)
		{
			ZigBee.E18_Baud = E18_Baud_38400;
			return 1;
		}else if(E18_RECEIVE_BUFF[5] == 0x00 && E18_RECEIVE_BUFF[6] == 0xE1)
		{
			ZigBee.E18_Baud = E18_Baud_57600;
			return 1;
		}else if(E18_RECEIVE_BUFF[7] == 0x01 && E18_RECEIVE_BUFF[6] == 0xC2)
		{
			ZigBee.E18_Baud = E18_Baud_115200;
			return 1;
		}
	}
	return 0;
}

/********************************************************
* Function name ：E18_Read_Target_ShortAddress
* Description   : 发送命令,获取ZigBee当前的短地址
* Parameter     ：None
* Return        ：0 -- fail  ,  1 -- success
**********************************************************/
uint8_t 	E18_Read_Target_ShortAddress(void)
{
	uint16_t tc = 0;
	uint8_t dat[8] = {0x55, 0x06, 0x00, 0x10, 0x00, 0x01, 0x00, 0x11};

	RX_NUM = 0;
	RX_NUM_LENGTH = 8 - 1;
	E18_CMD_FLAG = 1;

	ZigBee_Send_Data_length(dat, 8);

	HAL_UART_Receive_IT(E18_USART,(uint8_t *)UART4_temp, 1);
	while(E18_CMD_FLAG)
	{
		Delay_ms(1);
		tc++;
		if(tc > 2000)
		{
			return 0;
		}
	}
	
	E18_RECEIVE_BUFF[0] = 0;
	if(E18_RECEIVE_BUFF[4] == 0x00)
	{
		ZigBee.E18_Target_ShortAddress[0] = E18_RECEIVE_BUFF[5];
		ZigBee.E18_Target_ShortAddress[1] = E18_RECEIVE_BUFF[6];
		return 1;
	}
	return 0;
}

/********************************************************
* Function name ：E18_Read_Target_Port
* Description   : 发送命令,获取ZigBee当前的目标端口
* Parameter     ：None
* Return        ：0 -- fail  ,  1 -- success
**********************************************************/
uint8_t		E18_Read_Target_Port(void)
{
	uint16_t tc = 0;
	uint8_t dat[8] = {0x55, 0x06, 0x00, 0x10, 0x00, 0x02, 0x00, 0x12};

	RX_NUM = 0;
	RX_NUM_LENGTH = 7 - 1;
	E18_CMD_FLAG = 1;

	ZigBee_Send_Data_length(dat, 8);

	HAL_UART_Receive_IT(E18_USART,(uint8_t *)UART4_temp, 1);
	while(E18_CMD_FLAG)
	{
		Delay_ms(1);
		tc++;
		if(tc > 2000)
		{
			return 0;
		}
	}
	
	E18_RECEIVE_BUFF[0] = 0;
	if(E18_RECEIVE_BUFF[4] == 0x00)
	{
		ZigBee.E18_Port = E18_RECEIVE_BUFF[5];
		return 1;
	}
	return 0;
}

/********************************************************
* Function name ：E18_Read_LowPower
* Description   : 发送命令,获取ZigBee当前的低功耗模式
* Parameter     ：None
* Return        ：0 -- fail  ,  1 -- success
**********************************************************/
uint8_t 	E18_Read_LowPower(void)
{
	uint16_t tc = 0;
	uint8_t dat[8] = {0x55, 0x06, 0x00, 0x10, 0x00, 0x04, 0x00, 0x14};

	RX_NUM = 0;
	RX_NUM_LENGTH = 7 - 1;
	E18_CMD_FLAG = 1;

	ZigBee_Send_Data_length(dat, 8);

	HAL_UART_Receive_IT(E18_USART,(uint8_t *)UART4_temp, 1);
	while(E18_CMD_FLAG)
	{
		Delay_ms(1);
		tc++;
		if(tc > 2000)
		{
			return 0;
		}
	}
	
	E18_RECEIVE_BUFF[0] = 0;
	if(E18_RECEIVE_BUFF[4] == 0x00)
	{
		ZigBee.E18_LowPower_Grade = E18_RECEIVE_BUFF[5];
		return 1;
	}
	return 0;
}

/********************************************************
* Function name ：E18_Read_SerialPortMode
* Description   : 发送命令,获取ZigBee当前的串口发送模式
* Parameter     ：None
* Return        ：0 -- fail  ,  1 -- success
**********************************************************/
uint8_t	E18_Read_SerialPortMode(void)
{
	uint16_t tc = 0;
	uint8_t dat[8] = {0x55, 0x06, 0x00, 0x10, 0x00, 0x03, 0x00, 0x13};

	RX_NUM = 0;
	RX_NUM_LENGTH = 2 - 1;
	E18_CMD_FLAG = 1;

	ZigBee_Send_Data_length(dat, 8);

	HAL_UART_Receive_IT(E18_USART,(uint8_t *)UART4_temp, 1);
	while(E18_CMD_FLAG)
	{
		Delay_ms(1);
		tc++;
		if(tc > 2000)
		{
			printf("E18_Read_SerialPortMode TimeOut\n\r");
			return 0;
		}
	}
	

	if(E18_RECEIVE_BUFF[0] == 0x55)
	{
		ZigBee.E18_SerialPort_Mode = E18_CommandMode;
		E18_RECEIVE_BUFF[0] = 0x00;
		return 1;
	}else if(E18_RECEIVE_BUFF[0] == 0x4F){
		ZigBee.E18_SerialPort_Mode = E18_TransparentMode;
		E18_RECEIVE_BUFF[0] = 0x00;
		return 1;
	}
	return 0;
}

/********************************************************
* Function name ：E18_Set_DevType
* Description   : 发送命令,设置ZigBee当前的设备类型
* Parameter     ：type:ZigBee的设备类型
* Return        ：0 -- fail  ,  1 -- success
**********************************************************/
uint8_t E18_Set_DevType(E18_DEVType type)
{
	uint16_t tc = 0;
	uint8_t dat[6] = {0x55, 0x04, 0x00, 0x05, 0x02, 0x07};
	dat[4] = type;
	dat[5] = E18_Generate_CheckCode(3, dat);

	if(ZigBee.E18_Network_Status)
	{
		ZigBee_Send_Data_length(dat, 6);
	}else{
		return 1;
	}
	
	RX_NUM = 0;
	RX_NUM_LENGTH = 6 - 1;
	E18_CMD_FLAG = 1;

	HAL_UART_Receive_IT(E18_USART,(uint8_t *)UART4_temp, 1);
	while(E18_CMD_FLAG)
	{
		Delay_ms(1);
		tc++;
		if(tc > 2000)
		{
			printf("E18_Set_DevType TimeOut\n\r");
			return 0;
		}
	}
	
	E18_RECEIVE_BUFF[0] = 0;
	if(E18_RECEIVE_BUFF[4] == 0x00)
	{
		Delay_ms(100);
		while(!E18_Read_InitialState());
		if(ZigBee.E18_DevType == E18_Terminal){
			return 1;
		}else{
			return 0;
		}
	}
	return 0;
}

/********************************************************
* Function name ：E18_Set_Channel
* Description   : 发送命令,设置ZigBee当前的通信信道
* Parameter     ：channel：ZigBee的信道
* Return        ：0 -- fail  ,  1 -- success
**********************************************************/
uint8_t 	E18_Set_Channel(E18_CHANNEL channel)
{
	uint16_t tc = 0;
	uint8_t dat[7] = {0x55, 0x05, 0x00, 0x06, 0x01, 0x00, 0x12};
	dat[5] = channel;
	dat[6] = E18_Generate_CheckCode(4, dat);

	RX_NUM = 0;
	RX_NUM_LENGTH = 7 - 1;
	E18_CMD_FLAG = 1;

	if(ZigBee.E18_Network_Status)
	{
		ZigBee_Send_Data_length(dat, 7);
	}else{
		return 1;
	}

	HAL_UART_Receive_IT(E18_USART,(uint8_t *)UART4_temp, 1);
	while(E18_CMD_FLAG)
	{
		Delay_ms(1);
		tc++;
		if(tc > 2000)
		{
			return 0;
		}
	}
	
	E18_RECEIVE_BUFF[0] = 0;
	if(E18_RECEIVE_BUFF[4] == 0x00)
	{
		return 1;
	}
	return 0;
}

/********************************************************
* Function name ：E18_Set_PanID
* Description   : 发送命令,设置ZigBee当前的PANID
* Parameter     ：PanID_0：PADID的第零位；PanID_1：PANID的第一位
* Return        ：0 -- fail  ,  1 -- success
**********************************************************/
uint8_t 	E18_Set_PanID(uint8_t PanID_0, uint8_t PanID_1)
{
	uint16_t tc = 0;
	uint8_t dat[7] = {0x55, 0x05, 0x00, 0x08, 0x98, 0x89, 0x19};
	dat[4] = PanID_0;
	dat[5] = PanID_1;
	dat[6] = E18_Generate_CheckCode(4, dat);

	RX_NUM = 0;
	RX_NUM_LENGTH = 6 - 1;
	E18_CMD_FLAG = 1;

	if(ZigBee.E18_Network_Status)
	{
		ZigBee_Send_Data_length(dat, 7);
	}else{
		return 1;
	}

	HAL_UART_Receive_IT(E18_USART,(uint8_t *)UART4_temp, 1);
	while(E18_CMD_FLAG)
	{
		Delay_ms(1);
		tc++;
		if(tc > 2000)
		{
			return 0;
		}
	}
	
	E18_RECEIVE_BUFF[0] = 0;
	if(E18_RECEIVE_BUFF[4] == 0x00)
	{
		return 1;
	}
	return 0;
}

/********************************************************
* Function name ：E18_Set_FactoryMode
* Description   : 发送命令,设置ZigBee为工厂模式
* Parameter     ：channel：需要测试的信道；power：需要测试的发射功率
* Return        ：0 -- fail  ,  1 -- success
**********************************************************/
uint8_t 	E18_Set_FactoryMode(E18_CHANNEL channel, E18_POWER_GRADE power)
{
	uint16_t tc = 0;
	uint8_t dat[8] = {0x55, 0x06, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x10};
	dat[4] = channel;
	dat[5] = power;
	dat[7] = E18_Generate_CheckCode(5, dat);

	RX_NUM = 0;
	RX_NUM_LENGTH = 6 - 1;
	E18_CMD_FLAG = 1;

	if(ZigBee.E18_Network_Status)
	{
		ZigBee_Send_Data_length(dat, 8);
	}else{
		return 1;
	}

	HAL_UART_Receive_IT(E18_USART,(uint8_t *)UART4_temp, 1);
	while(E18_CMD_FLAG)
	{
		Delay_ms(1);
		tc++;
		if(tc > 2000)
		{
			return 0;
		}
	}
	
	E18_RECEIVE_BUFF[0] = 0;
	if(E18_RECEIVE_BUFF[4] == 0x00)
	{
		return 1;
	}
	return 0;
}

/********************************************************
* Function name ：E18_Set_Group
* Description   : 发送命令,设置ZigBee加入指定的组号
* Parameter     ：group_0：组号的第一位；group_1：组号的第一位
* Return        ：0 -- fail  ,  1 -- success
**********************************************************/
uint8_t 	E18_Set_Group(uint8_t group_0, uint8_t group_1)
{
	uint16_t tc = 0;
	uint8_t dat[8] = {0x55, 0x06, 0x00, 0x0A, 0x00, 0x00, 0x10, 0x1A};
	dat[5] = group_0;
	dat[6] = group_1;
	dat[7] = E18_Generate_CheckCode(5, dat);

	RX_NUM = 0;
	RX_NUM_LENGTH = 6 - 1;
	E18_CMD_FLAG = 1;

	ZigBee_Send_Data_length(dat, 8);

	HAL_UART_Receive_IT(E18_USART,(uint8_t *)UART4_temp, 1);
	while(E18_CMD_FLAG)
	{
		Delay_ms(1);
		tc++;
		if(tc > 2000)
		{
			printf("E18_Set_Group TimeOut\n\r");
			return 0;
		}
	}
	
	E18_RECEIVE_BUFF[0] = 0;
	if(E18_RECEIVE_BUFF[4] == 0x00 || E18_RECEIVE_BUFF[4] == 0xB8)
	{
		return 1;
	}
	return 0;
}

/********************************************************
* Function name ：E18_Set_Ungroup
* Description   : 发送命令,设置ZigBee退出指定的组号
* Parameter     ：group_0：组号的第一位；group_1：组号的第一位
* Return        ：0 -- fail  ,  1 -- success
**********************************************************/
uint8_t 	E18_Set_Ungroup(uint8_t group_0, uint8_t group_1)
{
	uint16_t tc = 0;
	uint8_t dat[8] = {0x55, 0x06, 0x00, 0x0B, 0x00, 0x00, 0x10, 0x1B};
	dat[5] = group_0;
	dat[6] = group_1;
	dat[7] = E18_Generate_CheckCode(5, dat);

	RX_NUM = 0;
	RX_NUM_LENGTH = 6 - 1;
	E18_CMD_FLAG = 1;

	if(!ZigBee.E18_Network_Status)
	{
		ZigBee_Send_Data_length(dat, 8);
	}else{
		return 1;
	}

	HAL_UART_Receive_IT(E18_USART,(uint8_t *)UART4_temp, 1);
	while(E18_CMD_FLAG)
	{
		Delay_ms(1);
		tc++;
		if(tc > 2000)
		{
			return 0;
		}
	}
	
	
	E18_RECEIVE_BUFF[0] = 0;
	if(E18_RECEIVE_BUFF[4] == 0x00)
	{
		return 1;
	}
	return 0;
}

/********************************************************
* Function name ：E18_Set_SerialPort_Baud
* Description   : 发送命令,设置ZigBee的串口为指定的波特率
* Parameter     ：baud：波特率
*                支持设置为9600 19200 38600 57600 115200
* Return        ：0 -- fail  ,  1 -- success
**********************************************************/
uint8_t	E18_Set_SerialPort_Baud(uint32_t baud)
{
	uint16_t tc = 0;
	uint8_t dat[12] = {0x55, 0x0A, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0xC2, 0x01, 0x00, 0xD2};
	switch(baud)
	{
		case 9600	:dat[7] = 0x80;dat[8] = 0x25;break;
		case 19200	:dat[7] = 0x00;dat[8] = 0x4B;break;
		case 38400	:dat[7] = 0x00;dat[8] = 0x96;break;
		case 57600	:dat[7] = 0x00;dat[8] = 0xE1;break;
		case 115200	:dat[9] = 0x01;dat[8] = 0xC2;break;
		default		:return 0;
	}
	dat[11] = E18_Generate_CheckCode(9, dat);

	RX_NUM = 0;
	RX_NUM_LENGTH = 6 - 1;
	E18_CMD_FLAG = 1;

	ZigBee_Send_Data_length(dat, 12);

	HAL_UART_Receive_IT(E18_USART,(uint8_t *)UART4_temp, 1);
	while(E18_CMD_FLAG)
	{
		Delay_ms(1);
		tc++;
		if(tc > 2000)
		{
			return 0;
		}
	}
	
	E18_RECEIVE_BUFF[0] = 0;
	if(E18_RECEIVE_BUFF[4] == 0x00)
	{
		return 1;
	}
	return 0;
}

/********************************************************
* Function name ：E18_Set_Target_ShortAddress
* Description   : 发送命令,设置ZigBee的目标短地址为指定的地址
* Parameter     ：shortAddress_0：短地址的第一位；
*                shortAddress_1：短地址的第一位
* Return        ：0 -- fail  ,  1 -- success
**********************************************************/
uint8_t	E18_Set_Target_ShortAddress(uint8_t shortAddress_0, uint8_t shortAddress_1)
{
	uint16_t tc = 0;
	uint8_t dat[10] = {0x55, 0x08, 0x00, 0x11, 0x00, 0x01, 0x00, 0x00, 0x00, 0x92};
	dat[7] = shortAddress_0;
	dat[8] = shortAddress_1;
	dat[9] = E18_Generate_CheckCode(7, dat);

	RX_NUM = 0;
	RX_NUM_LENGTH = 6 - 1;
	E18_CMD_FLAG = 1;

	ZigBee_Send_Data_length(dat, 10);

	HAL_UART_Receive_IT(E18_USART,(uint8_t *)UART4_temp, 1);
	while(E18_CMD_FLAG)
	{
		Delay_ms(1);
		tc++;
		if(tc > 2000)
		{
			printf("E18_Set_Target_ShortAddress TimeOut\n\r");
			return 0;
		}
	}
	
	E18_RECEIVE_BUFF[0] = 0;
	if(E18_RECEIVE_BUFF[4] == 0x00)
	{
		return 1;
	}
	return 0;
}

/********************************************************
* Function name ：E18_Set_Target_Port
* Description   : 发送命令,设置ZigBee的目标端口为指定的端口
* Parameter     ：port：目标端口
* Return        ：0 -- fail  ,  1 -- success
**********************************************************/
uint8_t	E18_Set_Target_Port(uint8_t port)
{
	uint16_t tc = 0;
	uint8_t dat[9] = {0x55, 0x07, 0x00, 0x11, 0x00, 0x02, 0x00, 0x00, 0x12};
	dat[7] = port;
	dat[8] = E18_Generate_CheckCode(6, dat);

	RX_NUM = 0;
	RX_NUM_LENGTH = 6 - 1;
	E18_CMD_FLAG = 1;

	ZigBee_Send_Data_length(dat, 9);

	HAL_UART_Receive_IT(E18_USART,(uint8_t *)UART4_temp, 1);
	while(E18_CMD_FLAG)
	{
		Delay_ms(1);
		tc++;
		if(tc > 2000)
		{
			printf("E18_Set_Target_Port TimeOut\n\r");
			return 0;
		}
	}
	
	
	E18_RECEIVE_BUFF[0] = 0;
	if(E18_RECEIVE_BUFF[4] == 0x00)
	{
		return 1;
	}
	return 0;
}

/********************************************************
* Function name ：E18_Set_TransparentMode
* Description   : 发送命令,设置ZigBee的串口发送模式为透传模式
* Parameter     ：None
* Return        ：0 -- fail  ,  1 -- success
**********************************************************/
uint8_t	E18_Set_TransparentMode(void)
{
	uint16_t tc = 0;
	uint8_t dat[9] = {0x55, 0x07, 0x00, 0x11, 0x00, 0x03, 0x00, 0x01, 0x13};

	RX_NUM = 0;
	RX_NUM_LENGTH = 6 - 1;
	E18_CMD_FLAG = 1;

	if(!ZigBee.E18_Network_Status)
	{
		ZigBee_Send_Data_length(dat, 9);
	}else{
		return 1;
	}

	HAL_UART_Receive_IT(E18_USART,(uint8_t *)UART4_temp, 1);
	while(E18_CMD_FLAG)
	{
		Delay_ms(1);
		tc++;
		if(tc > 2000)
		{
			printf("E18_Set_TransparentMode TimeOut\n\r");
			return 0;
		}
	}
	
	
	E18_RECEIVE_BUFF[0] = 0;
	if(E18_RECEIVE_BUFF[4] == 0x00)
	{
		return 1;
	}
	return 0;
}

/********************************************************
* Function name ：E18_Set_LowPower
* Description   : 发送命令,设置ZigBee的目标短地址为指定的
* Parameter     ：lowPower：低功耗等级
* Return        ：0 -- fail  ,  1 -- success
**********************************************************/
uint8_t	E18_Set_LowPower(E18_LOWPOWER_GRADE lowPower)
{
	uint16_t tc = 0;
	uint8_t dat[9] = {0x55, 0x07, 0x00, 0x11, 0x00, 0x04, 0x00 ,0x00, 0x17};
	dat[7] = lowPower;
	dat[8] = E18_Generate_CheckCode(6, dat);

	RX_NUM = 0;
	RX_NUM_LENGTH = 6 - 1;
	E18_CMD_FLAG = 1;

	ZigBee_Send_Data_length(dat, 9);

	HAL_UART_Receive_IT(E18_USART,(uint8_t *)UART4_temp, 1);
	while(E18_CMD_FLAG)
	{
		Delay_ms(1);
		tc++;
		if(tc > 2000)
		{
			printf("E18_Set_LowPower TimeOut\n\r");
			return 0;
		}
	}
	
	
	E18_RECEIVE_BUFF[0] = 0;
	if(E18_RECEIVE_BUFF[4] == 0x00)
	{
		return 1;
	}
	return 0;
}

/********************************************************
* Function name ：E18_Set_Power
* Description   : 发送命令,设置ZigBee的发射功率为指定的功率
* Parameter     ：power：发射功率
* Return        ：0 -- fail  ,  1 -- success
**********************************************************/
uint8_t		E18_Set_Power(E18_POWER_GRADE power)
{
	uint16_t tc = 0;
	uint8_t dat[7] = {0x55, 0x05, 0x00, 0x0D, 0x01, 0x00, 0x1D};
	dat[5] = power;
	dat[6] = E18_Generate_CheckCode(4, dat);

	RX_NUM = 0;
	RX_NUM_LENGTH = 7 - 1;
	E18_CMD_FLAG = 1;

	ZigBee_Send_Data_length(dat, 7);

	HAL_UART_Receive_IT(E18_USART,(uint8_t *)UART4_temp, 1);
	while(E18_CMD_FLAG)
	{
		Delay_ms(1);
		tc++;
		if(tc > 2000)
		{
			printf("E18_Set_Power TimeOut\n\r");
			return 0;
		}
	}
	
	
	E18_RECEIVE_BUFF[0] = 0;
	if(E18_RECEIVE_BUFF[4] == 0x00)
	{
		return 1;
	}
	return 0;	
}

/********************************************************
* Function name ：E18_Set_AutoConnect
* Description   : 发送命令,设置ZigBee模组自动建立常连接
* Parameter     ：Node
* Return        ：0 -- fail  ,  1 -- success
**********************************************************/
uint8_t	E18_Set_AutoConnect(void)
{
	uint16_t tc = 0;
	uint8_t dat[6] = {0x55, 0x04, 0x00, 0x14, 0x00, 0x14};

	RX_NUM = 0;
	RX_NUM_LENGTH = 6 - 1;
	E18_CMD_FLAG = 1;

	ZigBee_Send_Data_length(dat, 6);

	HAL_UART_Receive_IT(E18_USART,(uint8_t *)UART4_temp, 1);
	while(E18_CMD_FLAG)
	{
		Delay_ms(1);
		tc++;
		if(tc > 2000)
		{
			return 0;
		}
	}
	
	
	E18_RECEIVE_BUFF[0] = 0;
	if(E18_RECEIVE_BUFF[4] == 0x00)
	{
		return 1;
	}
	return 0;
}

/********************************************************
* Function name ：E18_Set_CommandMode
* Description   : 发送命令,设置ZigBee串口发送模式为命令模式
* Parameter     ：None
* Return        ：None
**********************************************************/
void E18_Set_CommandMode(void)
{
	uint8_t dat[3] = {'+', '+', '+'};
	if(ZigBee.E18_SerialPort_Mode == E18_TransparentMode){
		ZigBee_Send_Data_length(dat, 3);
	}
}

/********************************************************
* Function name ：E18_Generate_CheckCode
* Description   : 生成校验码
* Parameter     ：len：需要参加计算的数据数量；
*                dat：需要生成校验码的命令数组
* Return        ：生成的校验码
**********************************************************/
uint8_t		E18_Generate_CheckCode(uint8_t len, uint8_t *dat)
{
	uint8_t i;
	uint8_t temp = 0x00;
	for(i = 0; i < len; i++)
	{
		temp = temp ^ dat[2 + i];
	}

	return temp;
}

/********************************************************
* Function name ：ZigBee_Send_Data_length
* Description   : 将串口发送函数封装，发送前的两行代码是为了解决HAL库
*                  未知的问题
* Parameter     ：str：需要发送的数据；
*                length_num：发送的数据长度
* Return        ：None
**********************************************************/
void ZigBee_Send_Data_length(uint8_t *str,uint8_t length_num)
{
        __HAL_UART_DISABLE(E18_USART);
        __HAL_UART_ENABLE(E18_USART);
    HAL_UART_Transmit(E18_USART, str, length_num, 999);
}

/********************************************************
* Function name ：E18_OrganizationalData
* Description   : 构造需要发送的传感器数据，并发送
* Parameter     ：None
* Return        ：None
**********************************************************/
void       E18_OrganizationalData()
{
    float batteryLevel = 0;
    uint8_t buf[5] = {0};
    Read_BatteryLevel();
    batteryLevel = Return_BatteryLevel();
    DHT11_Read_Data(buf);
    GY30_Read();
    E18_Send_DataBuf[0] = 0x4B;
    E18_Send_DataBuf[1] = 0x4B;
    E18_Send_DataBuf[2] = 0x55;
    E18_Send_DataBuf[3] = 0x55;
    E18_Send_DataBuf[4] = Node_ID;
    E18_Send_DataBuf[5] = VoltTransformToPercentage(batteryLevel*PARTIAL_PRESSURE_COEFFICIENT);
    E18_Send_DataBuf[6] = 0x03;
    E18_Send_DataBuf[7] = TEMPERATURE;
    E18_Send_DataBuf[8] = buf[2];
    E18_Send_DataBuf[9] = buf[3];
    E18_Send_DataBuf[10] = HUMIDITY;
    E18_Send_DataBuf[11] = buf[0];
    E18_Send_DataBuf[12] = buf[1];
    E18_Send_DataBuf[13] = LIGHT_INTENSITY;
    E18_Send_DataBuf[14] = GY30_ReturnLight();
    E18_Send_DataBuf[15] = 0x00;
    E18_Send_DataBuf[16] = 0x55;
    E18_Send_DataBuf[17] = 0x55;
    E18_Send_DataBuf[18] = 0x4B;
    E18_Send_DataBuf[19] = 0x4B;
    ZigBee_Send_Data_length(E18_Send_DataBuf, 20);
}

/********************************************************
* Function name ：E18_CheckNodeControlCMD
* Description   : 向上级节点查询是否有控制自身的命令
* Parameter     ：Node
* Return        ：0 -- fail  ,  1 -- success
**********************************************************/
uint8_t       E18_CheckNodeControlCMD()
{
    uint16_t tc = 0;
    uint8_t dat[5] = {0x1F, 0x1F, Node_ID, 0xF1, 0xF1};

    RX_NUM = 0;
    RX_NUM_LENGTH = 6 - 1;
    E18_CMD_FLAG = 1;

    ZigBee_Send_Data_length(dat, 5);

    HAL_UART_Receive_IT(E18_USART,(uint8_t *)UART4_temp, 1);
    while(E18_CMD_FLAG)
    {
        Delay_ms(1);
        tc++;
        if(tc > 2000)
        {
            return 0;
        }
    }

    HAL_UART_Transmit(&huart1, E18_RECEIVE_BUFF, 8, 0xFF);
    E18_RECEIVE_BUFF[0] = 0;
    if(E18_RECEIVE_BUFF[4] == Node_ID)
    {
        if(E18_RECEIVE_BUFF[5] == 0x00){
            E18_NodeControlCMD = 0;
            HAL_UART_Transmit(&huart1, "E18_NodeControlCMD = 0", strlen("E18_NodeControlCMD = 0"), 0xFF);
        }else if(E18_RECEIVE_BUFF[5] == 0x01){
            E18_NodeControlCMD = 1;
            HAL_UART_Transmit(&huart1, "E18_NodeControlCMD = 1", strlen("E18_NodeControlCMD = 1"), 0xFF);
        }
        return 1;
    } else{
        return 0;
    }
}