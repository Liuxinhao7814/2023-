/********************************************************************************
* @File name: loginactivity.c
* @Author: fufeng
* @Version: 1.0
* @Date: 2023-6-22
* @brief   这个文件提供了登录相关的界面和处理逻辑
********************************************************************************/

#include "loginactivity.h"
#include "../ZigBee/zigbee.h"
#include <QGuiApplication>
#include <QScreen>
#include <QMessageBox>


LoginActivity::LoginActivity(QWidget *parert)
            : QWidget(parert)
{
    layoutInit();
    input[0]->setText("1079864036");
    input[1]->setText("111111");
}

LoginActivity::~LoginActivity()
{

}

/**
 *@brief 初始化布局
 *@param None
 *@return None
 */
void LoginActivity::layoutInit()
{
    QList <QScreen *> list_screen =  QGuiApplication::screens();

    /* 如果是ARM平台，直接设置大小为屏幕的大小 */
#if __arm__
    /* 重设大小 */
    this->resize(list_screen.at(0)->geometry().width(),
                 list_screen.at(0)->geometry().height());
#else
    /* 否则则设置主窗体大小为800x480 */
    this->resize(500, 400);
#endif

    loginWidget = new QWidget(this);
    loginVLayout = new QVBoxLayout();
    inputWidget = new QWidget();
    inputVLayout = new QVBoxLayout();

    QList <QString>list;
    list<<"Sign in to S-Management"<<"Username"<<"Password";
    for(int i = 0;i < 3; i++){
        label[i] = new QLabel();
        label[i]->setText(list[i]);
        vSpacer[i] = new QSpacerItem(10, 10,QSizePolicy::Minimum,QSizePolicy::Expanding);
    }
    for(int i = 0; i < 2; i++){
        input[i] = new QLineEdit();
    }
    enter = new QPushButton("Sign in");

    inputVLayout->addWidget(label[1]);
    inputVLayout->addWidget(input[0]);
    inputVLayout->addSpacerItem(vSpacer[0]);
    inputVLayout->addWidget(label[2]);
    inputVLayout->addWidget(input[1]);
    inputWidget->setLayout(inputVLayout);

    loginVLayout->addWidget(label[0]);
    loginVLayout->addSpacerItem(vSpacer[1]);
    loginVLayout->addWidget(inputWidget);
    loginVLayout->addSpacerItem(vSpacer[2]);
    loginVLayout->addWidget(enter);
    loginWidget->setLayout(loginVLayout);
    loginWidget->setContentsMargins(160, 50, 100, 100);


    connect(enter, SIGNAL(clicked()),this, SLOT(SignInClicked()));
}

/**
 *@brief 校验输入的信息是否正确
 *@param None
 *@return 0：false other：true
 */
int LoginActivity::checkInputData()
{
    clientID = input[0]->text();
    password = input[1]->text();

    if(clientID == "")
    {
         QMessageBox::warning(this, tr("提示"), tr("Username不能为空"), QMessageBox::Yes);
         input[0]->clear();
         input[0]->setFocus();
    }
    else if(password == "")
    {
         QMessageBox::warning(this, tr("提示"), tr("Password不能为空"), QMessageBox::Yes);
         input[0]->clear();
         input[0]->setFocus();
    }
    else if(clientID == "1079864036" && password == "111111")
    {
        QMessageBox::about(NULL, "Tip","Sign in is Success");
        return 1;
    }
    else if(clientID == "1079864036" && password != "111111")
    {
        QMessageBox::warning(this, tr("提示"), tr("Password is Error"), QMessageBox::Yes);
    }
    else if(clientID != "1079864036" && password == "111111")
    {
        QMessageBox::warning(this, tr("提示"), tr("Username is Error"), QMessageBox::Yes);
    }
    else if(clientID != "1079864036" && password != "111111")
    {
        QMessageBox::warning(this, tr("提示"), tr("Username And Password is Error"));
    }

    return 0;
}

/**
 *@brief 设置QButton的点击事件
 *@param None
 *@return None
 */
void LoginActivity::SignInClicked()
{
    if(checkInputData())
    {
        this->close();
        w = new MainWindow(clientID, password);
        w->show();
    }
}
