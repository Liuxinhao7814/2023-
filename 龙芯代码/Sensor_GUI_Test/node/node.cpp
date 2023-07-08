#include "node.h"
#include <unistd.h>
#include <QDebug>



Node::Node(QObject *parent, QString ID, int index)
{
    Q_UNUSED(parent)
    this->ID = ID;
    this->index = index;
    tabWidget = new QTabWidget();
    label = new QLabel();
    label->setText(QString("%1          电量：%2").arg(this->ID, "%"));
    labelItem = new QListWidgetItem();

    createExcel();

    timer = new QTimer();
    timer->setInterval(6000);
    timer->start();
    connect(timer, SIGNAL(timeout()), this, SLOT(resetStateFlag()));
}


Node::~Node()
{
//    delete *sensor;
//    delete tabWidget;
}

/**
 *@brief 创建数据储存文件
 *@param None
 *@return None
 */
void Node::createExcel()
{
    //QTextCodec::setCodecForTr(QTextCodec::codecForName("GBK"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));  //添加中文支持
    QString path = QDir::cleanPath(QCoreApplication::applicationDirPath() + QDir::separator() + (QString)"/outputFile"); //获取当前程序运行的目录，并添加
    fileName = path + "/" +this->getID() + ".csv";
    QFile csvFile(fileName);

    if(!csvFile.open(QIODevice::WriteOnly|QFile::Append)){
        qDebug() << this->ID + " " + "openExcel fail" << Qt::endl;
        exit(0);
    }

    csvFile.write("ID,Time,Battery Level/%,");
    csvFile.close();
}

/**
 *@brief 向数据储存文件添加表头
 *@param type 传感器类型
 *@return None
 */
void Node::addHeaderLine(QString type)
{
    QFile csvFile(fileName);
    if(!csvFile.open(QIODevice::WriteOnly|QFile::Append)){
        qDebug() << this->ID + " " + "openExcel fail" << Qt::endl;
        exit(0);
    }

    QString headerLine = type + "," + type + "状态,";
    char*  ch;
    QByteArray ba = headerLine.toLocal8Bit();  //将QString转为char*
    ch = ba.data();

    csvFile.write(ch);
    csvFile.close();
}

/**
 *@brief 向数据储存文件添加数据
 *@param data
 *@return None
 */
void Node::addDataToExcel(QString data)
{
    QFile csvFile(fileName);
    if(!csvFile.open(QIODevice::WriteOnly|QFile::Append)){
        qDebug() << this->ID + " " + "openExcel fail" << Qt::endl;
        exit(0);
    }

    data += ",";
    char*  ch;
    QByteArray ba = data.toLocal8Bit();  //将QString转为char*
    ch = ba.data();

    csvFile.write(ch);
    csvFile.close();
}

/**
 *@brief 向数据储存文件添加换行
 *@param None
 *@return None
 */
void Node::addWrap()
{
    QFile csvFile(fileName);
    if(!csvFile.open(QIODevice::WriteOnly|QFile::Append)){
        qDebug() << this->ID + " " + "openExcel fail" << Qt::endl;
        exit(0);
    }

    csvFile.write("\n");
    csvFile.close();
}

/**
 *
 */
void Node::resetStateFlag()
{
    setState(0);
}
