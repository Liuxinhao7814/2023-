/********************************************************************************
* @File name: onenet.h
* @Author: fufeng
* @Version: 1.0
* @Date: 2023-6-22
* @brief   This file contains all the function prototypes for the onenet.c file
********************************************************************************/

#ifndef ONENET_H
#define ONENET_H

#include <QObject>
#include "qmqtt.h"

//Ebf9HMecvdGb1XGJrAeWj=9Pfkk=
#define MQTT_Password "111111"
#define MQTT_WillMessage(X) ((X == 1) ? (MQTT_WillMessage_UPLine) : (MQTT_WillMessage_DownLine))

static const QString LoongsonID = "LoongSon0";
static const QString MQTT_WillMessage_UPLine = LoongsonID + "_UPLine";
static const QString MQTT_WillMessage_DownLine = LoongsonID + "_DownLine";
static const QString TopicData = "/Data";
static const QString MQTT_TopicLoongSonSync = "LoongSonSync";
static const QString MQTT_TopicLoongSonSyncRequest = "LoongSonSyncRequest";
static const QString MQTT_ClientId = "1079864036";
static const QString MQTT_Username = "601964";
static const QString MQTT_TopicWill = "LoongSonWill";
static const QString MQTT_TopicNodeSyncRequest = LoongsonID + "/NodeSyncRequest";
static const QString MQTT_TopicNodeSync = LoongsonID + "/NodeSync";
static const QString MQTT_TopicControlNode = LoongsonID + "/ControlNodeCmd";
static const QString MQTT_TopicSyncNodeStatus = LoongsonID + "/SyncNodeStatus";

class OneNet : public QObject
{
    Q_OBJECT
public:
    explicit OneNet(QString clientID, QString password, QObject *parent = nullptr);
    ~OneNet();

    void sendDataToOneNet(QString topic, QByteArray data);

private:


    QMQTT::Client *client;


signals:
    void receiveDataFromOneNet(const QMQTT::Message&);


private slots:

    void connect_success_slot();

    void connect_error_slot(const QMQTT::ClientError error);

    void disconnect_slot();

    void received_slot(const QMQTT::Message& msg);
};

#endif // ONENET_H
