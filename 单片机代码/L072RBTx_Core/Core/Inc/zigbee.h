/********************************************************************************
* @File name: zigbee.h
* @Author: fufeng
* @Version: 1.0
* @Date: 2023-6-22
* @brief   This file contains all the function prototypes for the zigbee.c file
********************************************************************************/

#ifndef _ZIGBEE_H_
#define _ZIGBEE_H_
#include "main.h"
#include "usart.h"

#define bool char
#define E18_PANID_0 	0x55
#define	E18_PANID_1	    0x00
#define E18_USART		&huart4
#define Node_ID         0x03

/**
  * @brief   ZigBee设备类型定义
  */
typedef enum {
	E18_Coordinated 		= 0x00	,		//协调器
	E18_Route 				= 0x01	,			//路由
	E18_Terminal 			= 0x02	,		//终端
	E18_Dormancy_Terminal 	= 0x03		//休眠终端
}E18_DEVType;

/**
  * @brief   ZigBee串口波特率定义
  */
typedef enum {
	E18_Baud_9600	= 9600	,
	E18_Baud_19200	= 19200	,
	E18_Baud_38400	= 38400	,
	E18_Baud_57600	= 57600	,
	E18_Baud_115200	= 115200,
}E18_BaudRate;

/**
  * @brief   ZigBee串口发送模式定义
  */
typedef enum {
	E18_CommandMode 	= 0x00	,
	E18_TransparentMode = 0x01
}E18_SERIALPORTMODE;

/**
  * @brief   ZigBee低功耗类型定义
  */
typedef enum {
	E18_CYCLE_1S 		= 0x00	,
	E18_CYCLE_3_3S 		= 0x01	,
	E18_CYCLE_5S		= 0x02	,
	E18_ALWAYS_DORMANT	= 0x03	,
}E18_LOWPOWER_GRADE;

/**
  * @brief   ZigBee信道定义
  */
typedef enum {
    E18_Channel_Null 	= 0x00	,
	E18_Channel_11 		= 0x0B	,
	E18_Channel_12		= 0x0C	,
	E18_Channel_13 		= 0x0D	,
	E18_Channel_14		= 0x0E	,
	E18_Channel_15		= 0x0F	,
	E18_Channel_16		= 0x10	,
	E18_Channel_17 		= 0x11	,
	E18_Channel_18 		= 0x12	,
	E18_Channel_19 		= 0x13	,
	E18_Channel_20 		= 0x14	,
	E18_Channel_21 		= 0x15	,
	E18_Channel_22 		= 0x16	,
	E18_Channel_23 		= 0x17	,
	E18_Channel_24 		= 0x18	,
	E18_Channel_25 		= 0x19	,
	E18_Channel_26	 	= 0x1A	,
	E18_Channel_all 	= 0xFF
}E18_CHANNEL;

/**
  * @brief   ZigBee发射功率定义
  */
typedef enum {
	E18_POWER_00	= 0x00	,
	E18_POWER_01	= 0x01	,
	E18_POWER_02	= 0x02	,
	E18_POWER_03	= 0x03	,
	E18_POWER_04	= 0x04	,
	E18_POWER_05	= 0x05	,
	E18_POWER_06	= 0x06	,
	E18_POWER_07	= 0x07	,
	E18_POWER_08	= 0x08	,
	E18_POWER_09	= 0x09	,
	E18_POWER_10	= 0x0A	,
	E18_POWER_11	= 0x0B	,
	E18_POWER_12	= 0x0C	,
	E18_POWER_13 	= 0x0D	,
	E18_POWER_14 	= 0x0E	,
	E18_POWER_15 	= 0x0F	,
	E18_POWER_16 	= 0x10	,
	E18_POWER_17 	= 0x11	,
	E18_POWER_18 	= 0x12	,
	E18_POWER_19 	= 0x13	,
	E18_POWER_20	= 0x14	,
	E18_POWER_NULL 	= 0xFF
}E18_POWER_GRADE;

/**
  * @brief   ZigBee设备状态定义
  */
typedef struct {
	E18_DEVType			E18_DevType;
	uint8_t				E18_MAC[8];
	E18_CHANNEL			E18_Channel;
	uint8_t				E18_PANID[2];
	uint8_t				E18_ShortAddress[2];
	uint8_t				E18_Target_ShortAddress[2];
	uint8_t 			E18_Extension_ID[8];
	uint8_t 			E18_SecretKey[16];
	uint8_t 			E18_Port;
	uint8_t				E18_Group[2];
	E18_LOWPOWER_GRADE 	E18_LowPower_Grade;
	E18_POWER_GRADE 	E18_Power_Grade;
	E18_BaudRate 		E18_Baud;
	E18_SERIALPORTMODE	E18_SerialPort_Mode;
	bool				E18_Network_Status;
}E18_STATUS;

/**
  * @brief   全局变量定义
  */
extern uint8_t 	E18_RECEIVE_BUFF[100];
extern uint8_t 	RX_NUM;
extern uint8_t 	RX_NUM_LENGTH;
extern uint8_t 	E18_CMD_FLAG;
extern uint8_t UDP_HEAD[4];
extern uint8_t UDP_TAIL[4];
extern uint8_t  E18_Send_DataBuf[30];
extern bool E18_NodeControlCMD;


void E18_HARDWARE_Init(void);

uint8_t 	E18_Open_Network(void);
uint8_t 	E18_Close_Network(void);
uint8_t 	E18_Reset(void);
uint8_t 	E18_Leave(void);
uint8_t 	E18_Recovery(void);

uint8_t		E18_Read_InitialState(void);
uint8_t 	E18_Read_status(void);
uint8_t 	E18_Read_Channel(void);
uint8_t 	E18_Read_PanID(void);
uint8_t 	E18_Read_Group(void);
uint8_t 	E18_Read_Power(void);
uint8_t 	E18_Read_Baud(void);
uint8_t 	E18_Read_Target_ShortAddress(void);
uint8_t		E18_Read_Target_Port(void);
uint8_t 	E18_Read_LowPower(void);
uint8_t		E18_Read_SerialPortMode(void);

uint8_t 	E18_Set_DevType(E18_DEVType type);
uint8_t 	E18_Set_Channel(E18_CHANNEL channel);
uint8_t 	E18_Set_PanID(uint8_t PanID_0, uint8_t PanID_1);
uint8_t 	E18_Set_FactoryMode(E18_CHANNEL channel, E18_POWER_GRADE power);
uint8_t 	E18_Set_Group(uint8_t group_0, uint8_t group_1);
uint8_t 	E18_Set_Ungroup(uint8_t group_0, uint8_t group_1);
uint8_t		E18_Set_SerialPort_Baud(uint32_t baud);
uint8_t		E18_Set_Target_ShortAddress(uint8_t shortAddress_0, uint8_t shortAddress_1);
uint8_t		E18_Set_Target_Port(uint8_t port);
uint8_t		E18_Set_TransparentMode(void);
uint8_t		E18_Set_LowPower(E18_LOWPOWER_GRADE lowPower);
uint8_t		E18_Set_Power(E18_POWER_GRADE power);
uint8_t		E18_Set_AutoConnect(void);
void 		E18_Set_CommandMode(void);

uint8_t		E18_Generate_CheckCode(uint8_t len, uint8_t *dat);
void 		ZigBee_Send_Data_length(uint8_t *str,uint8_t length_num);
void        E18_OrganizationalData();
uint8_t     E18_CheckNodeControlCMD();

#endif //_ZIGBEE_H_

