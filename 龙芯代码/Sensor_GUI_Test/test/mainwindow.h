/********************************************************************************
* @File name: mainwindow.h
* @Author: fufeng
* @Version: 1.0
* @Date: 2023-6-22
* @brief   This file contains all the function prototypes for the mainwindow.c file
********************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>
#include <QTabWidget>
#include <QLabel>
#include <QTextBrowser>
#include <QPushButton>
#include <QTimer>
#include <QComboBox>
#include <QLineEdit>
#include "../ZigBee/zigbee.h"
#include "../node/node.h"
#include "../oneNet/onenet.h"
#include "../Smtp/smtp.h"
#include "../Sms/sms.h"

class ZigBee;
class Node;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QString clientID, QString password, QWidget *parent = nullptr);
    ~MainWindow();

    QString getTime(){
        return this->time;
    }
    void setTime(QString time){
        this->time = time;
    }

    void addNodeLayout(QString ID, int nodeIndex);
    void addSensorLayout(QString type, int nodeIndex, int sensorIndex);


    void deleteNode(int);
    void addDataToLog(QString);

    QStringList  nodeIndex;     //可用于node的索引,因为被索引选中的node并不会被消除，只是释放内存，
    QStringList  nodeWidgetIndex; //可用于nodeWidget的索引
private:
    ZigBee *zigbee;
    OneNet *onenet;
    Smtp *smtp;
    Sms *sms;
    QWidget *mainWidget;
    QVBoxLayout *mainHLayout;
    QWidget *topWidget;
    QHBoxLayout *topHLayout;
    QWidget *dataWidget;
    QHBoxLayout *dataHLayout;
    QListWidget *nodeWidget;
    QStackedWidget *stackWidget;
    QComboBox *cmdSelect[2];
    QPushButton *cmdSend[2];
    QLabel *cmdReturn;
    QLineEdit *cmdInput;
    QTimer *timer;//用于定时查询节点状态
    QString time;
    float ComparisonValue[3];



    QString logName;
    Node *node[50];
    uint8_t cmdTemp[5];
    bool ok;

    void mainLayoutInit();
    void createLog();
    void zigbeeReadLayout();
    void zigbeeSetLayout();
    void zigbeeCmdInputCheck(int);
    QString checkSensorType(QString typeHex);
    QString checkunit(QString typeHex);

    void updataReferenceValue(QString typeHex, float data);
signals:
    void sendExceptionsToEmail(QString);
private slots:
    void updateTime();
    void checkReadData(QStringList);

    void cmdSendClicked_Read();
    void cmdSendClicked_Set();
    void cmdTips(int);

    void receiveDataFromOneNet(const QMQTT::Message&);
    void returnStatus(int);

};
#endif // MAINWINDOW_H

