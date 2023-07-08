QT       += core gui serialport charts network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    callout.cpp \
    chartview.cpp \
    loginactivity.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    callout.h \
    chartview.h \
    loginactivity.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
include(../ZigBee/ZigBee.pri)
include(../node/Node.pri)
include(../sensor/Sensor.pri)
include(../oneNet/oneNet.pri)
include(../Smtp/Smtp.pri)

unix:!macx: LIBS += -L$$PWD/lib/ -lQt5Qmqtt

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include
