/********************************************************************************
* @File name: sensor.h
* @Author: fufeng
* @Version: 1.0
* @Date: 2023-6-22
* @brief   This file contains all the function prototypes for the sensor.c file
********************************************************************************/

#ifndef SENSOR_H
#define SENSOR_H

#include <QObject>
#include <QChart>
#include <QSplineSeries>
#include <QDateTimeAxis>
#include <QValueAxis>
#include <QChartView>
#include <QVector>
#include <math.h>
#include <QLineSeries>
#include "chartview.h"
#include "callout.h"

QT_CHARTS_USE_NAMESPACE

class Sensor : public QObject
{
    Q_OBJECT
public:
    explicit Sensor(QObject *parent = nullptr);
    ~Sensor();

    void setType(QString type){
        chart->axisY()->setTitleText(type);
        this->type = type;
    }
    void setData(float floatData, float ComparisonValue){
        this->floatData = floatData;
        this->strData = QString::asprintf("%.2f", floatData);

        setState(checkSensorData(ComparisonValue));
        emit dataChanged(floatData);
    }
    void setState(bool state){
        this->state = state;
    }

    void setRoute(QString parentNode){
        this->route = parentNode+"/"+this->type;
    }

    void setSensorName(QString sensorName){
        this->sensorName = sensorName;
        if(sensorName == "温度"){
            minData = -20;
            maxData = 60;
            maxDiff = 10;
        }else if(sensorName == "湿度"){
            minData = 5;
            maxData = 95;
            maxDiff = 10;
        }else if(sensorName == "光照强度"){
            minData = 0;
            maxData = 200;
            maxDiff = 100;
        }
        /****其余传感器可根据需求添加***/
    }


    QString getSensorName(){
        return this->sensorName;
    }

    QString getRoute(){
        return this->route;
    }

    QString getType(){
        return this->type;
    }
    QString getStrData(){
        return strData;
    }
    float getFloatData(){
        return floatData;
    }
    QString getState(){
        if(this->state){
            return "正常";
        }else{
            return "异常";
        }
    }
    QChartView *getChartView(){
        return chartView;
    }

    bool checkSensorData(float data){
        float diff = 0;
        if(floatData > data){
            diff = floatData - data;
        }else if(floatData < data){
            diff = data - floatData;
        }

        if(diff > maxDiff){
            if(exceptionsFlag){
                emit sendExceptionsToEmail(route+"  传感器数据异常！\r\n"+"请立即处理");
                exceptionsFlag = 0;
            }
            return 0;
        }else {
            exceptionsFlag = 1;
            return 1;
        }
    }

    void initChart();
private:
    QString strData = nullptr;
    float floatData;
    QString type = nullptr;
    QString sensorName = nullptr;
    bool state;
    QString route = nullptr;
    bool exceptionsFlag = 0;
    float minData = 10;
    float maxData = 90;
    float maxDiff = 0;

    ChartView *chartView;
    QChart *chart;
    Callout *tip;
    QLineSeries *series;



signals:
    void dataChanged(float);
    void sendExceptionsToEmail(QString);


private slots:
    void addDataToChart(float);
};

#endif // SENSOR_H
