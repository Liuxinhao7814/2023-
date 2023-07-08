#include "sms.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QDebug>
#include <QUrlQuery>

#if 1
    // 移动短信接口
QString Sms::_sdkAppId = "lxhandwsw";
QString Sms::_sdkAppKey = "0dc6dc9110ec4088b63c434a7dc074ff";
#endif

Sms::Sms(QObject *parent)
    : QObject(parent),
      _random("0"),
      _type("0")
{
    connect(&_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(finished(QNetworkReply*)));
    connect(&_manager, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), this, SLOT(sslErrors(QNetworkReply*,QList<QSslError>)));
}

void Sms::singleMessage(QString mobile, QString content)
{
    // 移动平台  http://sms.sms.cn/
    /*
     *  http://api.sms.cn/sms/?ac=send
        &uid=qq21497936&pwd=接口密码[获取密码]
        &template=100006&mobile=填写要发送的手机号
        &content={"code":"value"}
    */
    QString baseUrl = QString("http://api.sms.cn/sms/");
    QUrl url;
    url.setUrl(baseUrl);
    QUrlQuery urlQuery;
    urlQuery.addQueryItem("ac", "send");
    // 笔者平台账户
    urlQuery.addQueryItem("uid", _sdkAppId);
    // 笔者平台账户接口密码
    urlQuery.addQueryItem("pwd", _sdkAppKey);
    urlQuery.addQueryItem("template", "100006");
    urlQuery.addQueryItem("mobile", mobile);
    urlQuery.addQueryItem("content", QString("{\"code\":\"%1\"}").arg(content));
    url.setQuery(urlQuery);
    _manager.get(QNetworkRequest(url));
}

void Sms::finished(QNetworkReply *reply)
{
    QByteArray data = reply->readAll();
    qDebug() << __FILE__ << __LINE__ << data;
#if 1
    // 移动短信收到的json可以判断是否发送成功，Demo未做这块
    "{\"stat\":\"100\",\"message\":\"\xB7\xA2\xCB\xCD\xB3\xC9\xB9\xA6\"}";
#endif
}

void Sms::sslErrors(QNetworkReply *reply, QList<QSslError> list)
{
    qDebug() << __FILE__ << __LINE__ << list;
}
