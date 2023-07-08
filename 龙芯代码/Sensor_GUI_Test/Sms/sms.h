#ifndef SMS_H
#define SMS_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSslError>

/************************************************************\
 * 类名：Sms
 * 描述：短信发送
 * 函数：
 *      singleMessage() - 单发短信，输入号码和发送的验证码
 * 作者：红模仿    QQ:21497936
 *       日期         版本       说明
 *   2018年4月26日   v1.0.0  使用http://sms.sms.cn/平台发送点对点短信
\************************************************************/

class Sms : public QObject
{
    Q_OBJECT
public:
    explicit Sms(QObject *parent = nullptr);

signals:
    void error(QString msg);

public slots:
    // 单发短信：给用户发短信验证码、短信通知，营销短息（内容长度不超过450字节）
    void singleMessage(QString number, QString content);

protected slots:
    void finished(QNetworkReply *);
    void sslErrors(QNetworkReply*,QList<QSslError>);

private:
    QNetworkAccessManager _manager;
    static QString _sdkAppId;
    static QString _sdkAppKey;
    QString _time;
    QString _random;
    QString _type;
};

#endif // SMS_H
