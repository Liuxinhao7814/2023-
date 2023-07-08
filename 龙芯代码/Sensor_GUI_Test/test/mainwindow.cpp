/********************************************************************************
* @File name: mainwindow.c
* @Author: fufeng
* @Version: 1.0
* @Date: 2023-6-22
* @brief   这个文件提供了主界面绘制和数据处理
********************************************************************************/

#include "mainwindow.h"
#include <QDateTime>
#include <QScreen>
#include <QGuiApplication>
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QString clientID, QString password, QWidget *parent)
    : QMainWindow(parent)
{
    zigbee = new ZigBee();
    onenet = new OneNet(clientID, password);
    smtp = new Smtp ();
    connect(smtp, SIGNAL(emailStatus(int)), this, SLOT(returnStatus(int)));

    connect(onenet,SIGNAL(receiveDataFromOneNet(const QMQTT::Message&)),this,SLOT(receiveDataFromOneNet(const QMQTT::Message&)));
    mainLayoutInit();
    createLog();
}

MainWindow::~MainWindow()
{

}

/**
 *@brief 初始化主界面布局控件
 *@param None
 *@return None
 */
void MainWindow::mainLayoutInit()
{
    QList <QScreen *> list_screen =  QGuiApplication::screens();

     /* 如果是ARM平台，直接设置大小为屏幕的大小 */
 #if __arm__
     /* 重设大小 */
     this->resize(list_screen.at(0)->geometry().width(),
                  list_screen.at(0)->geometry().height());
 #else
     /* 否则则设置主窗体大小为800x480 */
     this->resize(1024, 768);
 #endif
    mainWidget = new QWidget(this);
    mainHLayout = new QVBoxLayout();
    topWidget = new QWidget();
    topHLayout = new QHBoxLayout();
    dataWidget = new QWidget();
    dataHLayout = new QHBoxLayout();
    nodeWidget = new QListWidget();
    stackWidget = new QStackedWidget();
    timer = new QTimer();
    cmdReturn = new QLabel();

    cmdReturn->setMinimumSize(80, 30);
    cmdReturn->setText("I'm Return");

    zigbeeReadLayout();
    topHLayout->addWidget(cmdReturn);
    zigbeeSetLayout();
    topWidget->setLayout(topHLayout);

    nodeWidget->setMaximumWidth(200);
    dataHLayout->addWidget(nodeWidget);
    dataHLayout->addWidget(stackWidget);
    dataWidget->setLayout(dataHLayout);

    mainHLayout->addWidget(topWidget);    //用于留出顶部空间
    mainHLayout->addWidget(dataWidget);
    mainWidget->setLayout(mainHLayout);
    this->setCentralWidget(mainWidget);


    connect(nodeWidget, SIGNAL(currentRowChanged(int)),
            stackWidget, SLOT(setCurrentIndex(int)));
    connect(zigbee, SIGNAL(readDataChanged(QStringList)),
            this, SLOT(checkReadData(QStringList)));
    timer->setInterval(1000);
    timer->start();
    connect(timer, SIGNAL(timeout()),
            this, SLOT(updateTime()));
    connect(cmdSend[0], SIGNAL(clicked()),
            this, SLOT(cmdSendClicked_Read()));
    connect(cmdSend[1], SIGNAL(clicked()),
            this, SLOT(cmdSendClicked_Set()));
}

/**
 *@brief 初始化ZigBee读取参数布局控件
 *@param None
 *@return None
 */
void MainWindow::zigbeeReadLayout()
{
    cmdSelect[0] = new QComboBox();
    cmdSend[0] = new QPushButton();
    cmdSelect[0]->setMaximumSize(180, 30);
    cmdSend[0]->setMaximumSize(80, 30);
    cmdSend[0]->setText("Read");

    QList <QString> list;
       list<<"                 ---空---"<<"E18_Read_status"<<"E18_Read_Channel"<<"E18_Read_PanID"
          <<"E18_Read_Group"<<"E18_Read_Power"<<"E18_Read_Baud"
         <<"E18_Read_Target_ShortAddress"<<"E18_Read_Target_Port"<<"E18_Read_LowPower"
        <<"E18_Read_SerialPortMode";
       for (int i = 0; i < 11; i++) {
           cmdSelect[0]->addItem(list[i]);
       }

    topHLayout->addWidget(cmdSelect[0]);
    topHLayout->addWidget(cmdSend[0]);
}

/**
 *@brief 初始化ZigBee设置参数布局控件
 *@param None
 *@return None
 */
void MainWindow::zigbeeSetLayout()
{
    cmdSelect[1] = new QComboBox();
    cmdSend[1] = new QPushButton();
    cmdInput = new QLineEdit();
    cmdSelect[1]->setMaximumSize(180, 30);
    cmdSend[1]->setMaximumSize(80, 30);
    cmdInput->setMaximumSize(300, 30);
    cmdSend[1]->setText("Set");
    cmdInput->setPlaceholderText("请输入正确的格式");
    QList <QString> list;
       list<<"                 ---空---"<<"E18_Set_DevType"<<"E18_Set_Channel"<<"E18_Set_PanID"<<"E18_Set_Group"
          <<"E18_Set_Ungroup"<<"E18_Set_SerialPort_Baud"<<"E18_Set_Target_ShortAddress"
         <<"E18_Set_Target_Port"<<"E18_Set_TransparentMode"<<"E18_Set_LowPower"
        <<"E18_Set_Power"<<"E18_Set_CommandMode"<<"E18_Open_Network"<<"E18_Close_Network"<<"E18_Reset"
       <<"E18_Leave"<<"E18_Recovery"<<"E18_Reset_Hardware"<<"E18_Reset_Bound"<<"E18_Set_AutoConnect"<<"E18_Set_FactoryMode";
       for (int i = 0; i < 22; i++) {
           cmdSelect[1]->addItem(list[i]);
       }

    topHLayout->addWidget(cmdSelect[1]);
    topHLayout->addWidget(cmdInput);
    topHLayout->addWidget(cmdSend[1]);
    connect(cmdSelect[1], SIGNAL(currentIndexChanged(int)),
            this, SLOT(cmdTips(int)));
}

/**
 *@brief 处理从ZigBee接收到的数据
 *@param mid 信号传递过来的十六进制数组
 *@return None
 */
void MainWindow::checkReadData(QStringList mid)
{
    //第一次获取到ID进行初始化，之后才添加数据
    if(mid[0] == "4B" && mid[1] == "4B" && mid[2] == "55" && mid[3] == "55"){
        if(mid[4] != "00"){    //判断ID存在与否
            if(!nodeWidgetIndex.contains(mid[4])){   //如果是新ID就创建相应的Layout
                addDataToLog(mid[4] + "  UpLine");
                if(!nodeIndex.contains(mid[4])){
                    nodeIndex.append(mid[4]);
                }
                nodeWidgetIndex.append(mid[4]);
                int indexNode = nodeIndex.indexOf(mid[4]);
                addNodeLayout(mid[4], indexNode);    //创建节点
                for(int i = 0; i < mid[6].toInt(); i++){   //根据传感器节点数量创建相关布局
                    addSensorLayout(mid[7 + i * 3], indexNode, i);
                    int intPart = mid[8 + i * 3].toInt(&ok,16);
                    float decPart = mid[9 + i * 3].toInt(&ok,16);
                    float data = intPart + decPart / 100;

                    updataReferenceValue(mid[7 + i * 3], data);
                }
                stackWidget->addWidget(node[indexNode]->getTabWidget());
                node[indexNode]->addWrap();
            }else {
                //如果节点存在，则更新判断其是否存在的计时器,并更新数据
                QString message;
                int index = nodeIndex.indexOf(mid[4]);
                message.append(LoongsonID);
                message.append("_"+node[index]->getID());
                node[index]->addDataToExcel(node[index]->getID());
                node[index]->addDataToExcel(this->getTime());
                node[index]->setState(1);

                int batteryLevel = mid[5].toInt(&ok,16);
                message.append("_"+QString::number(batteryLevel, 10));
                message.append("_"+QString::number(node[index]->getSensorNum(), 10));
                node[index]->setBatteryLevel(batteryLevel);
                node[index]->addDataToExcel(QString::number(node[index]->getBatteryLevel(), 10));
                for(int i = 0; i < mid[6].toInt(); i++){
                    int intPart = mid[8 + i * 3].toInt(&ok,16);
                    float decPart = mid[9 + i * 3].toInt(&ok,16);
                    float data = intPart + decPart / 100;
                    node[index]->sensor[i]->setData(data, ComparisonValue[i]);

                    message.append("_"+node[index]->sensor[i]->getType());
                    message.append("_"+node[index]->sensor[i]->getStrData());
                    node[index]->addDataToExcel(node[index]->sensor[i]->getStrData());
                    node[index]->addDataToExcel(node[index]->sensor[i]->getState());
                }
                node[index]->addWrap();
                QString topic = LoongsonID + "/" + node[index]->getID() + TopicData;
                onenet->sendDataToOneNet(topic, message.toUtf8());
            }
        }
    }else if(mid[0] == "1F" && mid[1] == "1F"){ //传感器节点查询是否有给自己的控制命令
        if(nodeWidgetIndex.contains(mid[2])){
             int index = nodeIndex.indexOf(mid[2]);
             if(node[index]->isCheckNodeControlCMD()){
                 zigbee->SendDataToNode(node[index]->getID(), node[index]->getNodeControlCMD());
                 node[index]->clearNodeControlCMD();
             }
        }
    }
}

/**
 *@brief 初始化传感器节点相关的布局
 *@param ID 传感器节点号
 *@param nodeIndex 节点索引
 *@return None
 */
void MainWindow::addNodeLayout(QString ID, int nodeIndex)
{
    node[nodeIndex] = new Node(0, ID, nodeIndex);
    node[nodeIndex]->setRoute(LoongsonID);
    node[nodeIndex]->setState(1);
    connect(node[nodeIndex],SIGNAL(sendExceptionsToEmail(QString)), smtp, SLOT(sendEmailSlot(QString)));

    nodeWidget->addItem(node[nodeIndex]->labelItem);
    nodeWidget->setItemWidget(node[nodeIndex]->labelItem, node[nodeIndex]->label);
}

/**
 *@brief 进行节点下线的操作
 *@param Index 节点索引
 *@return None
 */
void MainWindow::deleteNode(int Index)
{
    QString ID = node[Index]->getID();
    int nodeWidgetindex = nodeWidgetIndex.indexOf(ID);
    node[Index]->setWarning();
    //nodeWidgetIndex.removeOne(ID);     //删除对应的nodeWidget的索引缓存

    //stackWidget->removeWidget(node[Index]->getTabWidget());
    //delete node[Index];                         //删除Node和其传感器类变量
    //nodeWidget->takeItem(nodeWidgetindex);      //删除相关布局
    addDataToLog(ID + "  DownLine");
}

/**
 *@brief 初始化传感器节点相关的布局
 *@param type 十六进制的传感器类型
 *@param nodeIndex 节点索引
 *@param sensorIndex
 *@return None
 */
void MainWindow::addSensorLayout(QString type, int nodeIndex, int sensorIndex)
{
    QString sensorName,sensorType;
    sensorName = checkSensorType(type);
    sensorType = checkunit(type);

    node[nodeIndex]->addSensor(sensorType, sensorName, sensorIndex);
    connect(node[nodeIndex]->sensor[sensorIndex],SIGNAL(sendExceptionsToEmail(QString)), smtp, SLOT(sendEmailSlot(QString)));
    node[nodeIndex]->sensor[sensorIndex]->setRoute(node[nodeIndex]->getRoute());
    node[nodeIndex]->addHeaderLine(type);
    node[nodeIndex]->getTabWidget()->addTab(node[nodeIndex]->sensor[sensorIndex]->getChartView(), sensorName);
}

/**
 *@brief 将十六进制的传感器类型转为中文字段
 *@param typeHex 十六进制的传感器类型
 *@return 转换后的中文字段
 */
QString MainWindow::checkSensorType(QString typeHex){
    bool ok;
    switch(typeHex.toInt(&ok, 16)){  //转16进制int
    case 0xFF : return "温度";
    case 0xFE : return "湿度";
    case 0xFD : return "光照强度";
    case 0xFC : return "人体脉搏";
    case 0xFB : return "火焰监测";
    case 0xFA : return "霍尔磁场检测";
    case 0xF9 : return "红外测距";
    case 0xF8 : return "超声波测距";
    case 0xF7 : return "烟雾浓度";
    case 0xF6 : return "PM2.5浓度";
    case 0xF5 : return "加速度";
    case 0xF4 : return "气压";
    default:
        break;
    }
    return " ";
}

/**
 *@brief 将十六进制的传感器类型转为带单位的中文字段
 *@param typeHex 十六进制的传感器类型
 *@return 转换后的中文字段
 */
QString MainWindow::checkunit(QString typeHex)
{
    bool ok;
    switch(typeHex.toInt(&ok, 16)){  //转16进制int
    case 0xFF : return "温度/℃";
    case 0xFE : return "湿度/%RH";
    case 0xFD : return "光照强度/Lux";
    case 0xFC : return "人体脉搏";
    case 0xFB : return "火焰监测";
    case 0xFA : return "霍尔磁场检测/mT";
    case 0xF9 : return "红外测距/m";
    case 0xF8 : return "超声波测距/m";
    case 0xF7 : return "烟雾浓度/ppm";
    case 0xF6 : return "PM2.5浓度μg/m3";
    case 0xF5 : return "加速度";
    case 0xF4 : return "气压/Pa";
    default:
        break;
    }
    return " ";
}

/**
 *@brief 更新time变量，并check节点的状态
 *@param None
 *@return None
 */
void MainWindow::updateTime()
{
    QDateTime dateTime= QDateTime::currentDateTime();//获取系统当前的时间
    QString str = dateTime.toString("yyyy-MM-dd hh:mm:ss");//格式化时间
    setTime(str);

    /************检查所有节点的状态************/
    for(int i = 0; i < nodeWidgetIndex.length(); i++)   //范围由node数组的实际大小来确定
    {
        QString ID = nodeWidgetIndex[i];
        int indexNode = nodeIndex.indexOf(ID);
        if(!node[indexNode]->getState()){
            //告知安卓端有节点掉线
            QString msg = LoongsonID + "_" + node[indexNode]->getID()+"_0";
            onenet->sendDataToOneNet(MQTT_TopicSyncNodeStatus, msg.toUtf8());
            deleteNode(i);
        }
    }
}

/**
 *@brief 创建日志文件
 *@param None
 *@return None
 */
void MainWindow::createLog()
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));  //添加中文支持
    QString path = QDir::cleanPath(QCoreApplication::applicationDirPath() + QDir::separator() + (QString)"/outputFile"); //获取当前程序运行的目录，并添加
    QDir tempDir(path);
    if(!tempDir.exists()){
        bool ismkdir = tempDir.mkdir(path);
        if(!ismkdir)
            qDebug() << "Create path fail" << Qt::endl;
        else
            qDebug() << "Create fullpath success" << Qt::endl;
    }else{
        qDebug() << "fullpath exist" << Qt::endl;
    }
    logName = path + "/" + "idLog.txt";
    QFile logFile(logName);

    if(!logFile.open(QIODevice::WriteOnly|QFile::Append)){
        qDebug() << "openlog fail" << Qt::endl;
        exit(0);
    }

    logFile.close();
}

/**
 *@brief 向日志文件写入数据
 *@param data 需要写入的数据
 *@return None
 */
void MainWindow::addDataToLog(QString data)
{
    QFile logFile(logName);
    if(!logFile.open(QIODevice::WriteOnly|QFile::Append)){
        qDebug() << "openlog fail" << Qt::endl;
        exit(0);
    }

    char*  ch;
    QByteArray ba;

    ba= this->getTime().toLocal8Bit();  //将QString转为char*
    ch = ba.data();
    logFile.write(ch);

    logFile.write("  ");
    ba= data.toLocal8Bit();  //将QString转为char*
    ch = ba.data();
    logFile.write(ch);

    logFile.write("\n");
    logFile.close();
}

/**
 *@brief QButton点击事件，点击后获取索引执行相应的函数
 *@param None
 *@return None
 */
void MainWindow::cmdSendClicked_Read()
{
    uint8_t ret;
    switch(cmdSelect[0]->currentIndex())
    {
    case 1 : ret = zigbee->E18_Read_status();break;
    case 2 : ret = zigbee->E18_Read_Channel();break;
    case 3 : ret = zigbee->E18_Read_PanID();break;
    case 4 : ret = zigbee->E18_Read_Group();break;
    case 5 : ret = zigbee->E18_Read_Power();break;
    case 6 : ret = zigbee->E18_Read_Baud();break;
    case 7 : ret = zigbee->E18_Read_Target_ShortAddress();break;
    case 8 : ret = zigbee->E18_Read_Target_Port();break;
    case 9 : ret = zigbee->E18_Read_LowPower();break;
    case 10: ret = zigbee->E18_Read_SerialPortMode();break;
    default: ret = 0;break;
    }
    if(ret){
        cmdReturn->setText("读取成功");
    }else{
        cmdReturn->setText("读取失败");
    }
}

/**
 *@brief QButton点击事件，点击后获取索引执行相应的函数
 *@param None
 *@return None
 */
void MainWindow::cmdSendClicked_Set()
{
    uint8_t ret;
    int index = cmdSelect[1]->currentIndex();

    zigbeeCmdInputCheck(index);
    switch(index)
    {
    case 1 : ret = zigbee->E18_Set_DevType((E18_DEVType)cmdTemp[0]);break;
    case 2 : ret = zigbee->E18_Set_Channel((E18_CHANNEL)cmdTemp[0]);break;
    case 3 : ret = zigbee->E18_Set_PanID(cmdTemp[0], cmdTemp[1]);break;
    case 4 : ret = zigbee->E18_Set_Group(cmdTemp[0], cmdTemp[1]);break;
    case 5 : ret = zigbee->E18_Set_Ungroup(cmdTemp[0], cmdTemp[1]);break;
    case 6 : ret = zigbee->E18_Set_SerialPort_Baud(cmdTemp[0]);break;
    case 7 : ret = zigbee->E18_Set_Target_ShortAddress(cmdTemp[0], cmdTemp[1]);break;
    case 8 : ret = zigbee->E18_Set_Target_Port(cmdTemp[0]);break;
    case 9 : ret = zigbee->E18_Set_TransparentMode();break;
    case 10: ret = zigbee->E18_Set_LowPower((E18_LOWPOWER_GRADE)cmdTemp[0]);break;
    case 11: ret = zigbee->E18_Set_Power((E18_POWER_GRADE)cmdTemp[0]);break;
    case 12: ret = zigbee->E18_Set_CommandMode();break;
    case 13: ret = zigbee->E18_Open_Network();break;
    case 14: ret = zigbee->E18_Close_Network();break;
    case 15: ret = zigbee->E18_Reset();break;
    case 16: ret = zigbee->E18_Leave();break;
    case 17: ret = zigbee->E18_Recovery();break;
    case 18: ret = 0;/*zigbee->E18_Reset_Hardware()*/;break;
    case 19: ret = 0;/*zigbee->E18_Reset_Bound();*/break;
    case 20: ret = zigbee->E18_Set_AutoConnect();break;
    case 21: ret = zigbee->E18_Set_FactoryMode((E18_CHANNEL)cmdTemp[0], (E18_POWER_GRADE)cmdTemp[1]);break;
    default: ret = 0;break;
    }

    if(ret){
        cmdReturn->setText("设置成功");
    }else{
        cmdReturn->setText("设置失败");
    }
}

/**
 *@brief 校验输入的命令是否合法
 *@param index QComboBox下拉框的索引
 *@return None
 */
void MainWindow::zigbeeCmdInputCheck(int index)
{
    QStringList  mid;
    QString str, str1;
    bool ok;
    str = cmdInput->text();
    cmdInput->clear();
    for(int i = 0; i < str.length(); i += 2)
    {
        str1 = str.mid(i, 2);
        mid.append(str1);
    }
    int len = mid.length();
    switch(index)
    {
    case 1 :
        if(len == 1){
            cmdTemp[0] = mid[0].toInt(&ok,16);
            if(cmdTemp[0] > 0x03)
            {
                QMessageBox::about(NULL, "Tip","非法的数据");
            }
        }else{
            QMessageBox::about(NULL, "Tip","数据格式错误");
        }
        break;
    case 2 :if(len == 1){
            cmdTemp[0] = mid[0].toInt(&ok,16);
        }else{
            QMessageBox::about(NULL, "Tip","数据格式错误");
        }
        break;
    case 3 :if(len == 2){
            cmdTemp[0] = mid[0].toInt(&ok,16);
            cmdTemp[1] = mid[1].toInt(&ok,16);
        }else{
            QMessageBox::about(NULL, "Tip","数据格式错误");
        }
        break;
    case 4 :if(len == 2){
            cmdTemp[0] = mid[0].toInt(&ok,16);
            cmdTemp[1] = mid[1].toInt(&ok,16);
        }else{
            QMessageBox::about(NULL, "Tip","数据格式错误");
        }
        break;
    case 5 :if(len == 2){
            cmdTemp[0] = mid[0].toInt(&ok,16);
            cmdTemp[1] = mid[1].toInt(&ok,16);
        }else{
            QMessageBox::about(NULL, "Tip","数据格式错误");
        }
        break;
    case 6 :if(len <= 3){
            cmdTemp[0] = mid[0].toInt(&ok,16);
        }else{
            QMessageBox::about(NULL, "Tip","数据格式错误");
        }
        break;
    case 7 :if(len == 2){
            cmdTemp[0] = mid[0].toInt(&ok,16);
            cmdTemp[1] = mid[1].toInt(&ok,16);
        }else{
            QMessageBox::about(NULL, "Tip","数据格式错误");
        }
        break;
    case 8 :if(len == 1){
            cmdTemp[0] = mid[0].toInt(&ok,16);
        }else{
            QMessageBox::about(NULL, "Tip","数据格式错误");
        }
        break;
    case 9 :break;
    case 10:if(len == 1){
            cmdTemp[0] = mid[0].toInt(&ok,16);
        }else{
            QMessageBox::about(NULL, "Tip","数据格式错误");
        }
        break;
    case 11:if(len == 1){
            cmdTemp[0] = mid[0].toInt(&ok,16);
        }else{
            QMessageBox::about(NULL, "Tip","数据格式错误");
        }
        break;
    case 12:break;
    case 13:break;
    case 14:break;
    case 15:break;
    case 16:break;
    case 17:break;
    case 18:break;
    case 19:break;
    case 20:break;
    case 21:if(len == 2){
            cmdTemp[0] = mid[0].toInt(&ok,16);
            cmdTemp[1] = mid[1].toInt(&ok,16);
        }else{
            QMessageBox::about(NULL, "Tip","数据格式错误");
        }
        break;
    default:break;
    }
}

/**
 *@brief 根据选中的函数在输入框中显示提示
 *@param selectIndex QComboBox下拉框的索引
 *@return None
 */
void MainWindow::cmdTips(int selectIndex)
{
    switch(selectIndex)
    {
    case 1 : cmdInput->setPlaceholderText("Tips: (E18_DEVType)01");break;
    case 2 : cmdInput->setPlaceholderText("Tips: (E18_CHANNEL)19");break;
    case 3 : cmdInput->setPlaceholderText("Tips: (E18_PANID)1A47");break;
    case 4 : cmdInput->setPlaceholderText("Tips: (E18_Group)1122");break;
    case 5 : cmdInput->setPlaceholderText("Tips: (E18_Group)1122");break;
    case 6 : cmdInput->setPlaceholderText("Tips: (E18_BaudRate)115200");break;
    case 7 : cmdInput->setPlaceholderText("Tips: (E18_Target_ShortAddress)1122");break;
    case 8 : cmdInput->setPlaceholderText("Tips: (E18_Port)FF");break;
    case 9 : cmdInput->setPlaceholderText("Tips: None");break;
    case 10: cmdInput->setPlaceholderText("Tips: (E18_LOWPOWER_GRADE)00");break;
    case 11: cmdInput->setPlaceholderText("Tips: (E18_POWER_GRADE)0E");break;
    case 12: cmdInput->setPlaceholderText("Tips: None");break;
    case 13: cmdInput->setPlaceholderText("Tips: None");break;
    case 14: cmdInput->setPlaceholderText("Tips: None");break;
    case 15: cmdInput->setPlaceholderText("Tips: None");break;
    case 16: cmdInput->setPlaceholderText("Tips: None");break;
    case 17: cmdInput->setPlaceholderText("Tips: None");break;
    case 18: cmdInput->setPlaceholderText("Tips: None");break;
    case 19: cmdInput->setPlaceholderText("Tips: None");break;
    case 20: cmdInput->setPlaceholderText("Tips: None");break;
    case 21: cmdInput->setPlaceholderText("Tips: (E18_CHANNEL)19 (E18_POWER_GRADE) 0E");break;
    default: break;
    }
}

/**
 *@brief 处理从MQTT服务器收到的数据
 *@param msg 从MQTT服务器收到的数据
 *@return None
 */
void MainWindow::receiveDataFromOneNet(const QMQTT::Message& msg)
{
    QString strPayload(msg.payload());
    QString topic(msg.topic());

    if(topic == MQTT_TopicLoongSonSyncRequest && strPayload == "1"){
        onenet->sendDataToOneNet(MQTT_TopicLoongSonSync, LoongsonID.toUtf8());
    }else if(topic == MQTT_TopicNodeSyncRequest && strPayload == "1"){
        for(int i = 0; i < nodeWidgetIndex.length(); i++){
            QString ID = nodeWidgetIndex[i];
            int indexNode = nodeIndex.indexOf(ID);
            QString message = LoongsonID + "~" + node[indexNode]->getID() + "~" + QString::number(node[indexNode]->getSensorNum(), 10) + "~"+ node[indexNode]->getSensorNameArray();
            qDebug()<<message;
            onenet->sendDataToOneNet(MQTT_TopicNodeSync, message.toUtf8());
        }
        onenet->sendDataToOneNet(MQTT_TopicNodeSync, "1");
    }else if(topic == MQTT_TopicControlNode){
        //Tips:节点名_命令
        QStringList list = strPayload.split("_");
        QString NodeID = list[0];

        QString cmd = list[1];
        int indexNode = nodeIndex.indexOf(NodeID);
        node[indexNode]->setNodeControlCMD(cmd);
    }
    qDebug()<<topic+","+strPayload<<Qt::endl;
}

/**
 *@brief 处理从SMTP服务器收到的数据
 *@param status 邮箱返回的状态
 *@return None
 */
void MainWindow::returnStatus(int status)
{
    if(status == EMAIL_SUCCEED){
        QMessageBox::about(NULL, "Tip","Send Email is Successed");
    }else if(status == EMAIL_ERROR){
        QMessageBox::about(NULL, "Tip","Send is Failed");
    }
}

/**
 *@brief 初始化时更新正常数据的平均值
 *@param typeHex 传感器类型
 *@param data 传感器数据
 *@return None
 */
void MainWindow::updataReferenceValue(QString typeHex, float data)
{
    switch(typeHex.toInt(&ok, 16)){  //转16进制int
    case 0xFF :
        ComparisonValue[0] += data;
        if(ComparisonValue[0] != 0){
            ComparisonValue[0] /= 2;
        };
    case 0xFE :
        ComparisonValue[1] += data;
        if(ComparisonValue[1] != 0){
            ComparisonValue[1] /= 2;
        };
    case 0xFD :
        ComparisonValue[2] += data;
        if(ComparisonValue[2] != 0){
            ComparisonValue[2] /= 2;
        };
    default:
        break;
    }
}
