#include "onenet.h"

OneNet::OneNet(QString clientID, QString password, QObject *parent)
    : QObject{parent}
{
    //QNetworkProxy::setApplicationProxy(QNetworkProxy::NoProxy);
    client = new QMQTT::Client(QHostAddress("183.230.40.39"), 6002);
    client->setClientId(clientID);
    client->setUsername(MQTT_Username);
    client->setPassword(password.toLatin1());
    client->setCleanSession(true);
    client->setKeepAlive(120);
    client->setVersion(QMQTT::MQTTVersion::V3_1_1);
    /******设置遗嘱消息******/
    client->setWillTopic(MQTT_TopicWill);
    client->setWillQos(2);
    client->setWillRetain(true);
    client->setWillMessage(MQTT_WillMessage(0).toUtf8());
    /*********************/
    client->connectToHost();
    QMQTT::Message msg(4, MQTT_TopicWill, MQTT_WillMessage(1).toUtf8(), 1, false, true);
    client->publish(msg);
    connect(client,SIGNAL(connected()),this,SLOT(connect_success_slot()));
    connect(client,SIGNAL(received(const QMQTT::Message&)),this,SLOT(received_slot(const QMQTT::Message&)));
    connect(client,SIGNAL(error(const QMQTT::ClientError)),this,SLOT(connect_error_slot(const QMQTT::ClientError)));
    connect(client,SIGNAL(disconnected()),this,SLOT(disconnect_slot()));
}

OneNet::~OneNet()
{
    client->disconnectFromHost();
}

/**
 *@brief 向One NET平台的MQTT服务器发送数据
 *@param topic 指定的主题
 *@param data 需要发送的数据
 *@return None
 */
void OneNet::sendDataToOneNet(QString topic, QByteArray data)
{
    QMQTT::Message msg(4, topic, data, 1, false, true);
    client->publish(msg);
}

/**
 *@brief 从MQTT服务器接收到数据，通过信号与槽传递给MainWindow
 *@param msg 接收到的数据
 *@return None
 */
void OneNet::received_slot(const QMQTT::Message& msg){
    emit receiveDataFromOneNet(msg);
}

/**
 *@brief 连接MQTT服务器成功后订阅相应的主题
 *@param None
 *@return None
 */
void OneNet::connect_success_slot(){
    qDebug() << "connect success";
    client->subscribe(MQTT_TopicLoongSonSyncRequest, 2);
    client->subscribe(MQTT_TopicNodeSyncRequest, 2);
    client->subscribe(MQTT_TopicControlNode, 1);
}

void OneNet::connect_error_slot(const QMQTT::ClientError error){
    qDebug() << "error"<<error;
}

/**
 *@brief 连接MQTT服务器失败后相应的处理
 *@param None
 *@return None
 */
void OneNet::disconnect_slot(){
    qDebug() << "disconnect___";
}
