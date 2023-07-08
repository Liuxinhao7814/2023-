#include "zigbee.h"
#include "mainwindow.h"
#include "loginactivity.h"
#include <unistd.h>
#include <QDebug>

ZigBee::ZigBee(QObject *parent)
{
    Q_UNUSED(parent)
    serialportInint();
    E18_HARDWARE_Init();
}

ZigBee::~ZigBee()
{

}

/**
 *
 */
void ZigBee::serialportInint()
{
    serialPort = new QSerialPort(this);

    /* 设置串口名 */
    serialPort->setPortName("ttyS1");

    /* 设置波特率 */
    serialPort->setBaudRate(115200);

    /* 设置数据位数 */
    serialPort->setDataBits(QSerialPort::Data8);

    /* 设置奇偶校验 */
    serialPort->setParity(QSerialPort::NoParity);

    /* 设置停止位 */
    serialPort->setStopBits(QSerialPort::OneStop);

    /* 设置流控制 */
    serialPort->setFlowControl(QSerialPort::NoFlowControl);

    if (!serialPort->open(QIODevice::ReadWrite))
        qDebug()<<"串口无法打开！可能正在被使用！"<<Qt::endl;
    else {
        qDebug()<<"Open SerialPort Successful!"<<Qt::endl;
    }

    connect(serialPort, SIGNAL(readyRead()),this, SLOT(serialPortReadyRead()));
}

/**
 *
 */
void ZigBee::serialPortReadyRead()
{
    /* 接收缓冲区中读取数据 */
    QByteArray buf = serialPort->readAll();
    QString temp;
    QStringList  mid;
    QString str;

    temp = buf.toHex().toUpper();

    if(temp.length() == 16 && !readPanIDFlag){    //收数据缓冲区容量为8个字符，if接收的数据大于容量，需要拼接
        readDataBuffer.append(temp);
    }else{
        readDataBuffer.append(temp);
        qDebug()<<readDataBuffer;
        for(int i = 0; i < readDataBuffer.length(); i += 2)
        {
            str = readDataBuffer.mid(i, 2);
            mid.append(str);
        }
        readyReadFlag = 1;
        readDataBuffer.remove(0, readDataBuffer.length());
    }
    if(E18_CMD_FLAG && readyReadFlag)
    {
        E18_Receive_QStringToInt(mid);
        if(E18_RECEIVE_BUFF[0] == 0x55 || E18_RECEIVE_BUFF[0] == 0x4F)
        {
            E18_CMD_FLAG = 0;
        }
        readyReadFlag = 0;
    }else if(E18_ZigBee.E18_SerialPort_Mode == E18_TransparentMode && readyReadFlag){
        readyReadFlag = 0;
        emit readDataChanged(mid);
    }

}

/********************************************************
* Function name ：E18_HARDWARE_Init
* Description   : 初始化ZigBee
* Parameter     ：None
* Return        ：None
**********************************************************/
void ZigBee::E18_HARDWARE_Init()
{
//    /**********组播********/
//    while(!E18_Read_SerialPortMode());
//    Delay_MSec(100);
//    E18_Set_CommandMode();
//    Delay_MSec(1000);
//    while(!E18_Read_InitialState());
//    Delay_MSec(100);
//    while(!E18_Read_status());
//    Delay_MSec(100);
//    if(E18_ZigBee.E18_Network_Status == 0x00 && E18_ZigBee.E18_PANID[0] == E18_PANID_0 && E18_ZigBee.E18_PANID[1] == E18_PANID_1)
//    {
//        //while(!E18_Open_Network());//如果上电打开网络是ZigBee模块自己的特性，就把这行代码去掉
//        //Delay_MSec(5000);
//        while(!E18_Set_TransparentMode());
//    }else{
//        while(!E18_Recovery());
//        Delay_MSec(2000);
//        while(!E18_Set_DevType(E18_Terminal));
//        Delay_MSec(100);
//        while(!E18_Set_Power(E18_POWER_18));
//        Delay_MSec(100);
//        while(!E18_Set_LowPower(E18_CYCLE_1S));
//        Delay_MSec(100);
//        while(!E18_Reset());
//        Delay_MSec(2000);
//        while(!E18_Open_Network());
//        Delay_MSec(5000);
//        while(!E18_Read_InitialState());
//        Delay_MSec(100);
//        while(!E18_Set_Group(0x11, 0x22));
//        Delay_MSec(100);
//        while(!E18_Set_Target_ShortAddress(0x11, 0x22));
//        Delay_MSec(100);
//        while(!E18_Set_Target_Port(0xFF));
//        Delay_MSec(100);
//    }
}

/********************************************************
* Function name ：E18_Open_Network
* Description   : 向ZigBee发送打开网络的指令
* Parameter     ：None
* Return        ：0 -- fail  ,  1 -- success
**********************************************************/
uint8_t ZigBee::E18_Open_Network(void)
{
    uint16_t tc = 0;
    uint8_t cmd[5] = {0x55, 0x03, 0x00, 0x02, 0x02};
    uint8_t len = sizeof (cmd)/sizeof (uint8_t);
    QString temp;
    temp = E18_Cmd_IntToQString(len, cmd);
    QByteArray dat = QByteArray::fromHex(temp.toLatin1());

    RX_NUM_LENGTH = 5 - 1;
    E18_CMD_FLAG = 1;

    if(E18_ZigBee.E18_Network_Status)
    {
        serialPort->write(dat);
    }else{
        return 1;
    }
    while(E18_CMD_FLAG)
    {
        Delay_MSec(1);
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
* Function name ：E18_Close_Network
* Description   : 向ZigBee发送关闭网络的指令
* Parameter     ：None
* Return        ：0 -- fail  ,  1 -- success
**********************************************************/
uint8_t 	ZigBee::E18_Close_Network(void)
{
    uint16_t tc = 0;
    uint8_t cmd[5] = {0x55, 0x03, 0x00, 0x03, 0x03};
    uint8_t len = sizeof (cmd)/sizeof (uint8_t);
    QString temp;
    temp = E18_Cmd_IntToQString(len, cmd);
    QByteArray dat = QByteArray::fromHex(temp.toLatin1());

    RX_NUM_LENGTH = 5 - 1;
    E18_CMD_FLAG = 1;

    if(!E18_ZigBee.E18_Network_Status)
    {
        serialPort->write(dat);
    }else{
        return 1;
    }
    while(E18_CMD_FLAG)
    {
        Delay_MSec(1);
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
uint8_t 	ZigBee::E18_Reset(void)
{
    uint16_t tc = 0;
    uint8_t cmd[9] = {0x55, 0x07, 0x00, 0x04, 0x00, 0xFF, 0xFF, 0x00, 0xEF};
    uint8_t len = sizeof (cmd)/sizeof (uint8_t);
    QString temp;
    E18_Generate_CheckCode(len, cmd);
    temp = E18_Cmd_IntToQString(len, cmd);
    QByteArray dat = QByteArray::fromHex(temp.toLatin1());
    RX_NUM_LENGTH = 7 - 1;
    E18_CMD_FLAG = 1;

    serialPort->write(dat);
    while(E18_CMD_FLAG)
    {
        Delay_MSec(1);
        tc++;
        if(tc > 2000)
        {
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
uint8_t 	ZigBee::E18_Leave(void)
{
    uint16_t tc = 0;
    uint8_t cmd[9] = {0x55, 0x07, 0x00, 0x04, 0x01, 0xFF, 0xFF, 0x00, 0xEE};
    cmd[5] = E18_ZigBee.E18_PANID[0];
    cmd[6] = E18_ZigBee.E18_PANID[1];
    cmd[7] = E18_ZigBee.E18_Channel;
    uint8_t len = sizeof (cmd)/sizeof (uint8_t);
    QString temp;
    E18_Generate_CheckCode(len, cmd);
    temp = E18_Cmd_IntToQString(len, cmd);
    QByteArray dat = QByteArray::fromHex(temp.toLatin1());

    if(E18_ZigBee.E18_Network_Status)
    {
        return 1;
    }

    RX_NUM_LENGTH = 7 - 1;
    E18_CMD_FLAG = 1;

    serialPort->write(dat);
    while(E18_CMD_FLAG)
    {
        Delay_MSec(1);
        tc++;
        if(tc > 2000)
        {
            return 0;
        }
    }
    E18_RECEIVE_BUFF[0] = 0;
    if(E18_RECEIVE_BUFF[3] == 0x00)
    {
        Delay_MSec(100);
        while(!E18_Read_status());
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
uint8_t 	ZigBee::E18_Recovery(void)
{
    uint16_t tc = 0;
    uint8_t cmd[9] = {0x55, 0x07, 0x00, 0x04, 0x02, 0xFF, 0xFF, 0x00, 0xED};
    cmd[5] = E18_ZigBee.E18_PANID[0];
    cmd[6] = E18_ZigBee.E18_PANID[1];
    cmd[7] = E18_ZigBee.E18_Channel;
    uint8_t len = sizeof (cmd)/sizeof (uint8_t);
    QString temp;
    E18_Generate_CheckCode(len, cmd);
    temp = E18_Cmd_IntToQString(len, cmd);
    QByteArray dat = QByteArray::fromHex(temp.toLatin1());

    if(E18_ZigBee.E18_Network_Status)
    {
        return 1;
    }

    RX_NUM_LENGTH = 7 - 1;
    E18_CMD_FLAG = 1;

    serialPort->write(dat);
    while(E18_CMD_FLAG)
    {
        Delay_MSec(1);
        tc++;
        if(tc > 2000)
        {
            return 0;
        }
    }
    E18_RECEIVE_BUFF[0] = 0;
    if(E18_RECEIVE_BUFF[3] == 0x00)
    {
        Delay_MSec(100);
        while(!E18_Read_status());  //更新网络状态
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
uint8_t 	ZigBee::E18_Read_status(void)
{
    uint16_t tc = 0;
    uint8_t i;
    uint8_t cmd[5] = {0x55, 0x03, 0x00, 0x00, 0x00};
    uint8_t len = sizeof (cmd)/sizeof (uint8_t);
    QString temp;
    temp = E18_Cmd_IntToQString(len, cmd);
    QByteArray dat = QByteArray::fromHex(temp.toLatin1());

    E18_CMD_FLAG = 1;
    serialPort->write(dat);
    while(E18_CMD_FLAG)
    {
        Delay_MSec(1);
        tc++;
        if(tc > 2000)
        {
            return 0;
        }
    }
    E18_RECEIVE_BUFF[0] = 0;
    if(E18_RECEIVE_BUFF[4] == 0xFF)		//未组网
    {
        E18_ZigBee.E18_Network_Status = 1;
        E18_ZigBee.E18_DevType = (E18_DEVType)E18_RECEIVE_BUFF[5];
        for(i = 0; i < 8; i++)
        {
            E18_ZigBee.E18_MAC[i] = E18_RECEIVE_BUFF[6 + i];
        }
        return 1;
    }else if(E18_RECEIVE_BUFF[4] == 0x00){	//已组网
        E18_ZigBee.E18_Network_Status = 0;
        E18_ZigBee.E18_DevType = (E18_DEVType)E18_RECEIVE_BUFF[5];
        for(i = 0; i < 8; i++)
        {
            E18_ZigBee.E18_MAC[i] = E18_RECEIVE_BUFF[6 + i];
            E18_ZigBee.E18_Extension_ID[i] = E18_RECEIVE_BUFF[19 + i];
        }
        E18_ZigBee.E18_Channel 	= (E18_CHANNEL)E18_RECEIVE_BUFF[14];
        E18_ZigBee.E18_PANID[0] = E18_RECEIVE_BUFF[15];
        E18_ZigBee.E18_PANID[1] = E18_RECEIVE_BUFF[16];
        E18_ZigBee.E18_ShortAddress[0] = E18_RECEIVE_BUFF[17];
        E18_ZigBee.E18_ShortAddress[1] = E18_RECEIVE_BUFF[18];
        for(i = 0; i < 16; i++)
        {
            E18_ZigBee.E18_SecretKey[i] = E18_RECEIVE_BUFF[27 + i];
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
uint8_t ZigBee::E18_Read_Channel(void)
{
    uint16_t tc = 0;
    uint8_t cmd[6] = {0x55, 0x04, 0x00, 0x06, 0x00, 0x06};
    uint8_t len = sizeof (cmd)/sizeof (uint8_t);
    QString temp;
    temp = E18_Cmd_IntToQString(len, cmd);
    QByteArray dat = QByteArray::fromHex(temp.toLatin1());

    E18_CMD_FLAG = 1;

    if(E18_ZigBee.E18_Network_Status)
    {
        serialPort->write(dat);
    }else{
        return 1;
    }
    while(E18_CMD_FLAG)
    {
        Delay_MSec(1);
        tc++;
        if(tc > 2000)
        {
            return 0;
        }
    }
    E18_RECEIVE_BUFF[0] = 0;
    if(E18_RECEIVE_BUFF[4] == 0x00)
    {
        E18_ZigBee.E18_Channel = (E18_CHANNEL)E18_RECEIVE_BUFF[5];
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
uint8_t 	ZigBee::E18_Read_PanID(void)
{
    uint16_t tc = 0;
    uint8_t cmd[5] = {0x55, 0x03, 0x00, 0x07, 0x07};
    uint8_t len = sizeof (cmd)/sizeof (uint8_t);
    QString temp;
    temp = E18_Cmd_IntToQString(len, cmd);
    QByteArray dat = QByteArray::fromHex(temp.toLatin1());

    E18_CMD_FLAG = 1;
    readPanIDFlag = 1;

    if(!E18_ZigBee.E18_Network_Status)
    {
        serialPort->write(dat);
    }else{
        E18_CMD_FLAG = 0;
        readPanIDFlag = 0;
        return 1;
    }
    while(E18_CMD_FLAG)
    {
        Delay_MSec(1);
        tc++;
        if(tc > 3000)
        {
            E18_CMD_FLAG = 0;
            readPanIDFlag = 0;
            return 0;
        }
    }
    E18_RECEIVE_BUFF[0] = 0;
    readPanIDFlag = 1;
    if(E18_RECEIVE_BUFF[4] == 0x00)
    {
        E18_ZigBee.E18_PANID[0] = E18_RECEIVE_BUFF[5];
        E18_ZigBee.E18_PANID[1] = E18_RECEIVE_BUFF[6];
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
uint8_t 	ZigBee::E18_Read_Group(void)
{
    uint16_t tc = 0;
    uint8_t cmd[6] = {0x55, 0x04, 0x00, 0x09, 0x00, 0x09};
    uint8_t len = sizeof (cmd)/sizeof (uint8_t);
    QString temp;
    temp = E18_Cmd_IntToQString(len, cmd);
    QByteArray dat = QByteArray::fromHex(temp.toLatin1());

    RX_NUM_LENGTH = 9 - 1;
    E18_CMD_FLAG = 1;

    serialPort->write(dat);
    while(E18_CMD_FLAG)
    {
        Delay_MSec(1);
        tc++;
        if(tc > 2000)
        {
            return 0;
        }
    }
    E18_RECEIVE_BUFF[0] = 0;
    if(E18_RECEIVE_BUFF[4] == 0x00)
    {
        E18_ZigBee.E18_Group[0] = E18_RECEIVE_BUFF[6];
        E18_ZigBee.E18_Group[1] = E18_RECEIVE_BUFF[7];
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
uint8_t 	ZigBee::E18_Read_Power(void)
{
    uint16_t tc = 0;
    uint8_t cmd[6] = {0x55, 0x04, 0x00, 0x0D, 0x00, 0x0D};
    uint8_t len = sizeof (cmd)/sizeof (uint8_t);
    QString temp;
    temp = E18_Cmd_IntToQString(len, cmd);
    QByteArray dat = QByteArray::fromHex(temp.toLatin1());
    RX_NUM_LENGTH = 7 - 1;
    E18_CMD_FLAG = 1;

    serialPort->write(dat);
    while(E18_CMD_FLAG)
    {
        Delay_MSec(1);
        tc++;
        if(tc > 2000)
        {
            return 0;
        }
    }
    E18_RECEIVE_BUFF[0] = 0;
    if(E18_RECEIVE_BUFF[4] == 0x00)
    {
        E18_ZigBee.E18_Power_Grade = (E18_POWER_GRADE)E18_RECEIVE_BUFF[5];
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
uint8_t 	ZigBee::E18_Read_Baud(void)
{
    uint16_t tc = 0;
    uint8_t cmd[8] = {0x55, 0x06, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10};
    uint8_t len = sizeof (cmd)/sizeof (uint8_t);
    E18_Generate_CheckCode(len, cmd);
    QString temp;
    temp = E18_Cmd_IntToQString(len, cmd);
    QByteArray dat = QByteArray::fromHex(temp.toLatin1());

    RX_NUM_LENGTH = 10 - 1;
    E18_CMD_FLAG = 1;

    serialPort->write(dat);
    while(E18_CMD_FLAG)
    {
        Delay_MSec(1);
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
            E18_ZigBee.E18_Baud = E18_Baud_9600;
            return 1;
        }else if(E18_RECEIVE_BUFF[7] == 0x00 && E18_RECEIVE_BUFF[6] == 0x4B)
        {
            E18_ZigBee.E18_Baud = E18_Baud_19200;
            return 1;
        }else if(E18_RECEIVE_BUFF[7] == 0x00 && E18_RECEIVE_BUFF[6] == 0x96)
        {
            E18_ZigBee.E18_Baud = E18_Baud_38400;
            return 1;
        }else if(E18_RECEIVE_BUFF[7] == 0x00 && E18_RECEIVE_BUFF[6] == 0xE1)
        {
            E18_ZigBee.E18_Baud = E18_Baud_57600;
            return 1;
        }else if(E18_RECEIVE_BUFF[7] == 0x01 && E18_RECEIVE_BUFF[6] == 0xC2)
        {
            E18_ZigBee.E18_Baud = E18_Baud_115200;
            qDebug()<<E18_ZigBee.E18_Baud<<Qt::endl;
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
uint8_t 	ZigBee::E18_Read_Target_ShortAddress(void)
{
    uint16_t tc = 0;
    uint8_t cmd[8] = {0x55, 0x06, 0x00, 0x10, 0x00, 0x01, 0x00, 0x11};
    uint8_t len = sizeof (cmd)/sizeof (uint8_t);
    QString temp;
    temp = E18_Cmd_IntToQString(len, cmd);
    QByteArray dat = QByteArray::fromHex(temp.toLatin1());

    RX_NUM_LENGTH = 8 - 1;
    E18_CMD_FLAG = 1;

    serialPort->write(dat);
    while(E18_CMD_FLAG)
    {
        Delay_MSec(1);
        tc++;
        if(tc > 2000)
        {
            return 0;
        }
    }
    E18_RECEIVE_BUFF[0] = 0;
    if(E18_RECEIVE_BUFF[4] == 0x00)
    {
        E18_ZigBee.E18_Target_ShortAddress[0] = E18_RECEIVE_BUFF[5];
        E18_ZigBee.E18_Target_ShortAddress[1] = E18_RECEIVE_BUFF[6];
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
uint8_t		ZigBee::E18_Read_Target_Port(void)
{
    uint16_t tc = 0;
    uint8_t cmd[8] = {0x55, 0x06, 0x00, 0x10, 0x00, 0x02, 0x00, 0x12};
    uint8_t len = sizeof (cmd)/sizeof (uint8_t);
    QString temp;
    temp = E18_Cmd_IntToQString(len, cmd);
    QByteArray dat = QByteArray::fromHex(temp.toLatin1());

    RX_NUM_LENGTH = 7 - 1;
    E18_CMD_FLAG = 1;

    serialPort->write(dat);
    while(E18_CMD_FLAG)
    {
        Delay_MSec(1);
        tc++;
        if(tc > 2000)
        {
            return 0;
        }
    }
    E18_RECEIVE_BUFF[0] = 0;
    if(E18_RECEIVE_BUFF[4] == 0x00)
    {
        E18_ZigBee.E18_Port = E18_RECEIVE_BUFF[5];
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
uint8_t 	ZigBee::E18_Read_LowPower(void)
{
    uint16_t tc = 0;
    uint8_t cmd[8] = {0x55, 0x06, 0x00, 0x10, 0x00, 0x04, 0x00, 0x14};
    uint8_t len = sizeof (cmd)/sizeof (uint8_t);
    QString temp;
    temp = E18_Cmd_IntToQString(len, cmd);
    QByteArray dat = QByteArray::fromHex(temp.toLatin1());

    RX_NUM_LENGTH = 7 - 1;
    E18_CMD_FLAG = 1;

    serialPort->write(dat);
    while(E18_CMD_FLAG)
    {
        Delay_MSec(1);
        tc++;
        if(tc > 2000)
        {
            return 0;
        }
    }
    E18_RECEIVE_BUFF[0] = 0;
    if(E18_RECEIVE_BUFF[4] == 0x00)
    {
        E18_ZigBee.E18_LowPower_Grade = (E18_LOWPOWER_GRADE)E18_RECEIVE_BUFF[5];
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
uint8_t	ZigBee::E18_Read_SerialPortMode(void)
{
    uint16_t tc = 0;
    uint8_t cmd[8] = {0x55, 0x06, 0x00, 0x10, 0x00, 0x03, 0x00, 0x13};
    uint8_t len = sizeof (cmd)/sizeof (uint8_t);
    QString temp;
    temp = E18_Cmd_IntToQString(len, cmd);
    QByteArray dat = QByteArray::fromHex(temp.toLatin1());

    RX_NUM_LENGTH = 2 - 1;
    E18_CMD_FLAG = 1;

    serialPort->write(dat);
    while(E18_CMD_FLAG)
    {
        Delay_MSec(1);
        tc++;
        if(tc > 2000)
        {
            return 0;
        }
    }
    if(E18_RECEIVE_BUFF[0] == 0x55)
    {
        E18_ZigBee.E18_SerialPort_Mode = E18_CommandMode;
        E18_RECEIVE_BUFF[0] = 0;
        return 1;
    }else if(E18_RECEIVE_BUFF[0] == 0x4F){
        E18_ZigBee.E18_SerialPort_Mode = E18_TransparentMode;
        E18_RECEIVE_BUFF[0] = 0;
        return 1;
    }else{
        return 0;
    }
}

/********************************************************
* Function name ：E18_Set_DevType
* Description   : 发送命令,设置ZigBee当前的设备类型
* Parameter     ：type:ZigBee的设备类型
* Return        ：0 -- fail  ,  1 -- success
**********************************************************/
uint8_t ZigBee::E18_Set_DevType(E18_DEVType type)
{
    uint16_t tc = 0;
    uint8_t cmd[6] = {0x55, 0x04, 0x00, 0x05, 0x02, 0x07};
    cmd[4] = type;
    uint8_t len = sizeof (cmd)/sizeof (uint8_t);

    E18_Generate_CheckCode(len, cmd);
    QString temp;
    temp = E18_Cmd_IntToQString(len, cmd);
    QByteArray dat = QByteArray::fromHex(temp.toLatin1());

    RX_NUM_LENGTH = 6 - 1;
    E18_CMD_FLAG = 1;

    if(E18_ZigBee.E18_Network_Status)
    {
        serialPort->write(dat);
    }else{
        return 1;
    }

    while(E18_CMD_FLAG)
    {
        Delay_MSec(1);
        tc++;
        if(tc > 2000)
        {
            return 0;
        }
    }
    E18_RECEIVE_BUFF[0] = 0;
    if(E18_RECEIVE_BUFF[4] == 0x00)
    {
        Delay_MSec(100);
        while(!E18_Read_status());  //更新设备类型
        return 1;
    }
    return 0;
}

/********************************************************
* Function name ：E18_Set_Channel
* Description   : 发送命令,设置ZigBee当前的通信信道
* Parameter     ：channel：ZigBee的信道
* Return        ：0 -- fail  ,  1 -- success
**********************************************************/
uint8_t 	ZigBee::E18_Set_Channel(E18_CHANNEL channel)
{
    uint16_t tc = 0;
    uint8_t cmd[7] = {0x55, 0x05, 0x00, 0x06, 0x01, 0x00, 0x12};
    cmd[5] = channel;
    uint8_t len = sizeof (cmd)/sizeof (uint8_t);
    E18_Generate_CheckCode(len, cmd);
    QString temp;
    temp = E18_Cmd_IntToQString(len, cmd);
    QByteArray dat = QByteArray::fromHex(temp.toLatin1());

    RX_NUM_LENGTH = 7 - 1;
    E18_CMD_FLAG = 1;

    if(E18_ZigBee.E18_Network_Status)
    {
        serialPort->write(dat);
    }else{
        return 1;
    }
    while(E18_CMD_FLAG)
    {
        Delay_MSec(1);
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
uint8_t 	ZigBee::E18_Set_PanID(uint8_t PanID_0, uint8_t PanID_1)
{
    uint16_t tc = 0;
    uint8_t cmd[7] = {0x55, 0x05, 0x00, 0x08, 0x98, 0x89, 0x19};
    cmd[4] = PanID_0;
    cmd[5] = PanID_1;
    uint8_t len = sizeof (cmd)/sizeof (uint8_t);
    E18_Generate_CheckCode(len, cmd);
    QString temp;
    temp = E18_Cmd_IntToQString(len, cmd);
    QByteArray dat = QByteArray::fromHex(temp.toLatin1());

    RX_NUM_LENGTH = 6 - 1;
    E18_CMD_FLAG = 1;

    if(E18_ZigBee.E18_Network_Status)
    {
        serialPort->write(dat);
    }else{
        return 1;
    }
    while(E18_CMD_FLAG)
    {
        Delay_MSec(1);
        tc++;
        if(tc > 2000)
        {
            readPanIDFlag = 0;
            return 0;
        }
    }
    readPanIDFlag = 0;
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
uint8_t 	ZigBee::E18_Set_FactoryMode(E18_CHANNEL channel, E18_POWER_GRADE power)
{
    uint16_t tc = 0;
    uint8_t cmd[8] = {0x55, 0x06, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x10};
    cmd[4] = channel;
    cmd[5] = power;
    uint8_t len = sizeof (cmd)/sizeof (uint8_t);
    E18_Generate_CheckCode(len, cmd);
    QString temp;
    temp = E18_Cmd_IntToQString(len, cmd);
    QByteArray dat = QByteArray::fromHex(temp.toLatin1());

    RX_NUM_LENGTH = 6 - 1;
    E18_CMD_FLAG = 1;

    if(E18_ZigBee.E18_Network_Status)
    {
        serialPort->write(dat);
    }else{
        return 1;
    }
    while(E18_CMD_FLAG)
    {
        Delay_MSec(1);
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
uint8_t 	ZigBee::E18_Set_Group(uint8_t group_0, uint8_t group_1)
{
    uint16_t tc = 0;
    uint8_t cmd[8] = {0x55, 0x06, 0x00, 0x0A, 0x00, 0x00, 0x10, 0x1A};
    cmd[5] = group_0;
    cmd[6] = group_1;
    uint8_t len = sizeof (cmd)/sizeof (uint8_t);
    E18_Generate_CheckCode(len, cmd);
    QString temp;
    temp = E18_Cmd_IntToQString(len, cmd);
    QByteArray dat = QByteArray::fromHex(temp.toLatin1());

    RX_NUM_LENGTH = 6 - 1;
    E18_CMD_FLAG = 1;

    serialPort->write(dat);
    while(E18_CMD_FLAG)
    {
        Delay_MSec(1);
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
* Function name ：E18_Set_Ungroup
* Description   : 发送命令,设置ZigBee退出指定的组号
* Parameter     ：group_0：组号的第一位；group_1：组号的第一位
* Return        ：0 -- fail  ,  1 -- success
**********************************************************/
uint8_t 	ZigBee::E18_Set_Ungroup(uint8_t group_0, uint8_t group_1)
{
    uint16_t tc = 0;
    uint8_t cmd[8] = {0x55, 0x06, 0x00, 0x0B, 0x00, 0x00, 0x10, 0x1B};
    cmd[5] = group_0;
    cmd[6] = group_1;
    uint8_t len = sizeof (cmd)/sizeof (uint8_t);
    E18_Generate_CheckCode(len, cmd);
    QString temp;
    temp = E18_Cmd_IntToQString(len, cmd);
    QByteArray dat = QByteArray::fromHex(temp.toLatin1());

    RX_NUM_LENGTH = 6 - 1;
    E18_CMD_FLAG = 1;

    if(!E18_ZigBee.E18_Network_Status)
    {
        serialPort->write(dat);
    }else{
        return 1;
    }
    while(E18_CMD_FLAG)
    {
        Delay_MSec(1);
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
uint8_t	ZigBee::E18_Set_SerialPort_Baud(uint32_t baud)
{
    uint16_t tc = 0;
    uint8_t cmd[12] = {0x55, 0x0A, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0xC2, 0x01, 0x00, 0xD2};
    switch(baud)
    {
        case 96	:cmd[7] = 0x80;cmd[8] = 0x25;break;
        case 19	:cmd[7] = 0x00;cmd[8] = 0x4B;break;
        case 38	:cmd[7] = 0x00;cmd[8] = 0x96;break;
        case 57	:cmd[7] = 0x00;cmd[8] = 0xE1;break;
        case 11	:cmd[9] = 0x01;cmd[8] = 0xC2;break;
        default		:break;
    }
    uint8_t len = sizeof (cmd)/sizeof (uint8_t);
    E18_Generate_CheckCode(len, cmd);
    QString temp;
    temp = E18_Cmd_IntToQString(len, cmd);
    QByteArray dat = QByteArray::fromHex(temp.toLatin1());

    RX_NUM_LENGTH = 6 - 1;
    E18_CMD_FLAG = 1;

    serialPort->write(dat);
    while(E18_CMD_FLAG)
    {
        Delay_MSec(1);
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
uint8_t	ZigBee::E18_Set_Target_ShortAddress(uint8_t shortAddress_0, uint8_t shortAddress_1)
{
    uint16_t tc = 0;
    uint8_t cmd[10] = {0x55, 0x08, 0x00, 0x11, 0x00, 0x01, 0x00, 0x00, 0x00, 0x92};
    cmd[7] = shortAddress_0;
    cmd[8] = shortAddress_1;
    uint8_t len = sizeof (cmd)/sizeof (uint8_t);
    E18_Generate_CheckCode(len, cmd);
    QString temp;
    temp = E18_Cmd_IntToQString(len, cmd);
    QByteArray dat = QByteArray::fromHex(temp.toLatin1());

    RX_NUM_LENGTH = 6 - 1;
    E18_CMD_FLAG = 1;

    serialPort->write(dat);
    while(E18_CMD_FLAG)
    {
        Delay_MSec(1);
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
* Function name ：E18_Set_Target_Port
* Description   : 发送命令,设置ZigBee的目标端口为指定的端口
* Parameter     ：port：目标端口
* Return        ：0 -- fail  ,  1 -- success
**********************************************************/
uint8_t	ZigBee::E18_Set_Target_Port(uint8_t port)
{
    uint16_t tc = 0;
    uint8_t cmd[9] = {0x55, 0x07, 0x00, 0x11, 0x00, 0x02, 0x00, 0x00, 0x12};
    cmd[7] = port;
    uint8_t len = sizeof (cmd)/sizeof (uint8_t);
    E18_Generate_CheckCode(len, cmd);
    QString temp;
    temp = E18_Cmd_IntToQString(len, cmd);
    QByteArray dat = QByteArray::fromHex(temp.toLatin1());

    RX_NUM_LENGTH = 6 - 1;
    E18_CMD_FLAG = 1;

    serialPort->write(dat);
    while(E18_CMD_FLAG)
    {
        Delay_MSec(1);
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
* Function name ：E18_Set_TransparentMode
* Description   : 发送命令,设置ZigBee的串口发送模式为透传模式
* Parameter     ：None
* Return        ：0 -- fail  ,  1 -- success
**********************************************************/
uint8_t	ZigBee::E18_Set_TransparentMode(void)
{
    uint16_t tc = 0;
    uint8_t cmd[9] = {0x55, 0x07, 0x00, 0x11, 0x00, 0x03, 0x00, 0x01, 0x13};
    uint8_t len = sizeof (cmd)/sizeof (uint8_t);
    QString temp;
    temp = E18_Cmd_IntToQString(len, cmd);
    QByteArray dat = QByteArray::fromHex(temp.toLatin1());

    RX_NUM_LENGTH = 6 - 1;
    E18_CMD_FLAG = 1;

    if(!E18_ZigBee.E18_Network_Status)
    {
        serialPort->write(dat);
    }else{
        return 1;
    }
    while(E18_CMD_FLAG)
    {
        Delay_MSec(1);
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
* Function name ：E18_Set_LowPower
* Description   : 发送命令,设置ZigBee的目标短地址为指定的
* Parameter     ：lowPower：低功耗等级
* Return        ：0 -- fail  ,  1 -- success
**********************************************************/
uint8_t	ZigBee::E18_Set_LowPower(E18_LOWPOWER_GRADE lowPower)
{
    uint16_t tc = 0;
    uint8_t cmd[9] = {0x55, 0x07, 0x00, 0x11, 0x00, 0x04, 0x00 ,0x00, 0x17};
    cmd[7] = lowPower;
    uint8_t len = sizeof (cmd)/sizeof (uint8_t);
    E18_Generate_CheckCode(len, cmd);
    QString temp;
    temp = E18_Cmd_IntToQString(len, cmd);
    QByteArray dat = QByteArray::fromHex(temp.toLatin1());

    RX_NUM_LENGTH = 6 - 1;
    E18_CMD_FLAG = 1;

    serialPort->write(dat);
    while(E18_CMD_FLAG)
    {
        Delay_MSec(1);
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
* Function name ：E18_Set_Power
* Description   : 发送命令,设置ZigBee的发射功率为指定的功率
* Parameter     ：power：发射功率
* Return        ：0 -- fail  ,  1 -- success
**********************************************************/
uint8_t		ZigBee::E18_Set_Power(E18_POWER_GRADE power)
{
    uint16_t tc = 0;
    uint8_t cmd[7] = {0x55, 0x05, 0x00, 0x0D, 0x01, 0x00, 0x1D};
    cmd[5] = power;
    uint8_t len = sizeof (cmd)/sizeof (uint8_t);
    E18_Generate_CheckCode(len, cmd);
    QString temp;
    temp = E18_Cmd_IntToQString(len, cmd);
    QByteArray dat = QByteArray::fromHex(temp.toLatin1());

    RX_NUM_LENGTH = 7 - 1;
    E18_CMD_FLAG = 1;

    serialPort->write(dat);
    while(E18_CMD_FLAG)
    {
        Delay_MSec(1);
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
* Function name ：E18_Set_AutoConnect
* Description   : 发送命令,设置ZigBee模组自动建立常连接
* Parameter     ：Node
* Return        ：0 -- fail  ,  1 -- success
**********************************************************/
uint8_t	ZigBee::E18_Set_AutoConnect(void)
{
    uint16_t tc = 0;
    uint8_t cmd[6] = {0x55, 0x04, 0x00, 0x14, 0x00, 0x14};
    uint8_t len = sizeof (cmd)/sizeof (uint8_t);
    QString temp;
    temp = E18_Cmd_IntToQString(len, cmd);
    QByteArray dat = QByteArray::fromHex(temp.toLatin1());

    RX_NUM_LENGTH = 6 - 1;
    E18_CMD_FLAG = 1;

    serialPort->write(dat);
    while(E18_CMD_FLAG)
    {
        Delay_MSec(1);
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
* Return        0 -- fail  ,  1 -- success
**********************************************************/
uint8_t ZigBee::E18_Set_CommandMode(void)
{
    QByteArray  dat(std::begin<char>({'+', '+', '+'}), 3);
    if(E18_ZigBee.E18_SerialPort_Mode == E18_TransparentMode){
        serialPort->write(dat);
        return 1;
    }
     return 0;
}

/********************************************************
* Function name ：E18_Generate_CheckCode
* Description   : 生成校验码
* Parameter     ：len：需要参加计算的数据数量；
*                dat：需要生成校验码的命令数组
* Return        ：生成的校验码
**********************************************************/
void		ZigBee::E18_Generate_CheckCode(uint8_t len, uint8_t *dat)
{
    uint8_t i;
    int temp = 0;
    for(i = 0; i < len - 3; i++)
    {
        temp = temp ^ dat[2 + i];
    }
    dat[len - 1] = temp;
}

/********************************************************
* Function name ：E18_Cmd_IntToQString
* Description   : 将数据由int转为QString
* Parameter     ：len 需要转换的数据长度；
*                cmd 需要转换的数据
* Return        ：转换后的数据
**********************************************************/
QString     ZigBee::E18_Cmd_IntToQString(uint8_t len, uint8_t *cmd)
{
    QString temp;
    for(int i = 0; i < len; i++)
    {
        if(cmd[i] <= 0x0F)
        {
            temp.append(QString::number(0,16));
            temp.append(QString::number(cmd[i],16));
        }else{
            temp.append(QString::number(cmd[i],16));
        }

    }
    return temp;
}

/********************************************************
* Function name ：E18_Receive_QStringToInt
* Description   : 将数据由QStringList转为int
* Parameter     ：mid 需要转换的数据
* Return        ：转换后的数据
**********************************************************/
void     ZigBee::E18_Receive_QStringToInt(QStringList mid)
{
    int len = mid.length();
    for(int i = 0; i < len; i++)
    {
        E18_RECEIVE_BUFF[i] = mid[i].toInt(&ok,16);
    }
}

/********************************************************
* Function name ：Delay_MSec
* Description   : 延时函数
* Parameter     ：msec 延时的毫秒数
* Return        ：转换后的数据
**********************************************************/
void ZigBee::Delay_MSec(unsigned int msec)
{
    QEventLoop loop;//定义一个新的事件循环
    QTimer::singleShot(msec, &loop, SLOT(quit()));//创建单次定时器，槽函数为事件循环的退出函数
    loop.exec();//事件循环开始执行，程序会卡在这里，直到定时时间到，本循环被退出
}

/********************************************************
* Function name ：SendDataToNode
* Description   : 发送数据到传感器节点
* Parameter     ：msec 延时的毫秒数
* Return        ：转换后的数据
**********************************************************/
void ZigBee::SendDataToNode(QString nodeID, QString cmd)
{
    QString CMD;
    CMD.append("1F");
    CMD.append("1F");
    CMD.append(nodeID);
    CMD.append("0" + cmd);
    CMD.append("F1");
    CMD.append("F1");
    QByteArray dat = QByteArray::fromHex(CMD.toLatin1());
    serialPort->write(dat);
    qDebug()<<dat<<Qt::endl;
}





