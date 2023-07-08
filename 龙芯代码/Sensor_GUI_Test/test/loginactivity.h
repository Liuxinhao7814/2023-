/********************************************************************************
* @File name: loginactivity.h
* @Author: fufeng
* @Version: 1.0
* @Date: 2023-6-22
* @brief   This file contains all the function prototypes for the loginactivity.c file
********************************************************************************/


#ifndef LOGINACTIVITY_H
#define LOGINACTIVITY_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QSpacerItem>
#include "mainwindow.h"
//#include "../SQLite/sqlite.h"

class MainWindow;

class LoginActivity : public QWidget
{
    Q_OBJECT

public:
    LoginActivity(QWidget *parent = nullptr);
    ~LoginActivity();

private:
    MainWindow *w;

    QWidget *loginWidget;
    QVBoxLayout *loginVLayout;
    QWidget *inputWidget;
    QVBoxLayout *inputVLayout;
    QLabel *label[3];
    QLineEdit *input[2];
    QPushButton *enter;
    QString clientID, password;

    QSpacerItem *vSpacer[3];

    void layoutInit();
    int checkInputData();


private slots:
    void SignInClicked();
};


#endif // LOGINACTIVITY_H
