#include "sensor.h"
#include <QDebug>

Sensor::Sensor(QObject *parent)
    : QObject{parent}
{

    initChart();

    connect(this, SIGNAL(dataChanged(float)),this, SLOT(addDataToChart(float)));
}

Sensor::~Sensor()
{

}

/**
 *@brief 连接MQTT服务器失败后相应的处理
 *@param None
 *@return None
 */
void Sensor::initChart()
{
    chart = new QChart();
    series = new QLineSeries;

    chart->addSeries(series);

//    series->setUseOpenGL(true);

    chart->createDefaultAxes();
    chart->axisY()->setRange(minData, maxData);
    chart->axisX()->setRange(0, 99);

    chart->axisX()->setTitleFont(QFont("Microsoft YaHei", 10, QFont::Normal, true));
    chart->axisY()->setTitleFont(QFont("Microsoft YaHei", 10, QFont::Normal, true));
    chart->axisX()->setTitleText("Time/sec");
    chart->axisY()->setTitleText("unit");

    chart->axisX()->setGridLineVisible(false);
    chart->axisY()->setGridLineVisible(false);

    /* hide legend */
    chart->legend()->hide();

    chartView = new ChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

}

/**
 *@brief 添加数据到波形图
 *@param data 数据
 *@return None
 */
void Sensor::addDataToChart(float data)
{

    int i;
    QVector<QPointF> oldData = series->pointsVector();
    QVector<QPointF> newData;

    if (oldData.size() < 100) {
        newData = series->pointsVector();
    } else {
        /* 添加之前老的数据到新的vector中，不复制最前的数据，即每次替换前面的数据
         * 由于这里每次只添加1个数据，所以为1，使用时根据实际情况修改
         */
        for (i = 1; i < oldData.size(); ++i) {
            newData.append(QPointF(i - 1 , oldData.at(i).y()));
        }
    }

    qint64 size = newData.size();
    /* 添加一个数据*/
    newData.append(QPointF(1 + size, data));

    series->replace(newData);
}

