/********************************************************************************
* @File name: node.h
* @Author: fufeng
* @Version: 1.0
* @Date: 2023-6-22
* @brief   This file contains all the function prototypes for the node.c file
********************************************************************************/

#ifndef NODE_H
#define NODE_H

#include <QObject>
#include <QTabWidget>
#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include <QTimer>
#include "../sensor/sensor.h"
#include <QTextCodec>
#include <QLabel>
#include <QListWidgetItem>


class Sensor;

class Node : public QObject
{
    Q_OBJECT

public:
    Node(QObject *parent, QString ID, int index);
    ~Node();


    void setID(QString ID){
        this->ID = ID;
    }
    void setIndex(int index){
        this->index = index;
    }
    void setState(bool state){
        this->state = state;
        if(state){
            timer->start();
        }
    }

    void setBatteryLevel(int batteryLevel){
        this->batteryLevel = batteryLevel;
        checkBatteryLevel(batteryLevel);
        updateBatteryLevel(batteryLevel);
    }

    void setRoute(QString parentNode){
        this->route = parentNode+"/"+this->ID;
    }

    void setNodeControlCMD(QString nodeControlCMD){
        this->nodeControlCMD = nodeControlCMD;
    }

    QString getNodeControlCMD(){
        return this->nodeControlCMD;
    }

    QString getRoute(){
        return this->route;
    }

    int getBatteryLevel(){
        return this->batteryLevel;
    }

    int getSensorNum(){
        return this->sensorNum;
    }

    QString getID(){
        return this->ID;
    }
    int getIndex(){
        return this->index;
    }
    bool getState(){
        return this->state;
    }

    QTabWidget* getTabWidget(){
        return this->tabWidget;
    }

    QString getSensorNameArray(){
        return this->sensorNameArray;
    }

    void clearNodeControlCMD(){
        this->nodeControlCMD = nullptr;
    }

    bool isCheckNodeControlCMD(){
        if(this->nodeControlCMD == nullptr){
            return false;
        }else{
            return true;
        }
    }

    void addSensor(QString type,QString sensorName,int sensorIndex){
        sensor[sensorIndex] = new Sensor();
        sensor[sensorIndex]->setSensorName(sensorName);
        sensor[sensorIndex]->setType(type);
        if(this->sensorNum == 0){
            this->sensorNameArray.append(type);
        }else{
            this->sensorNameArray.append("_");
            this->sensorNameArray.append(type);
        }

        this->sensorNum++;
    }

    void addDataToExcel(QString);
    void addHeaderLine(QString);
    void addWrap();
    void updateBatteryLevel(int batteryLevel){
        label->setText(QString("%1          电量：%2%").arg(this->ID, QString::number(batteryLevel, 10)));
    }

    void setWarning(){
        label->setText(QString("%1          Node DownLine!").arg(this->ID));
    }

    QTabWidget *tabWidget;
    QLabel *label;
    QListWidgetItem *labelItem;
    Sensor *sensor[5];
private:
    QString ID;
    int batteryLevel;
    int index;
    bool state;
    QString nodeControlCMD = nullptr;
    QString fileName;
    QString route = nullptr;
    QTimer *timer;
    int sensorNum = 0;
    QString sensorNameArray = nullptr;
    bool exceptionsFlag = 1;
    void createExcel();
    bool checkBatteryLevel(int batteryLevel){
        if(batteryLevel < 60){
            if(exceptionsFlag){
                emit sendExceptionsToEmail(route+"  电量过低！\r\n"+"请立即处理");
            }
            exceptionsFlag = 0;
            return 0;
        }else{
            exceptionsFlag = 1;
            return 1;
        }
    }


signals:
    void sendExceptionsToEmail(QString);
private slots:
    void resetStateFlag();

};

#endif // NODE_H
