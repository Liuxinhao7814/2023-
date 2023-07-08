/**
 *author @Eyre Turing
 *by https://blog.csdn.net/nicedante/article/details/102637590
 */

#ifndef Smtp_H
#define Smtp_H

#include <QObject>
#include <QTcpSocket>

#define EMAIL_ERROR     0   //邮件发送失败
#define EMAIL_SUCCEED   1   //邮件发送成功
static const QString SMTP_FromEmailNumber = "1839346873@qq.com";
static const QString SMTP_ToEmailNumber = "1839346873@qq.com";
static const QString SMTP_Password = "sevtpalmrzwadgfj";
static const QString SMTP_ExceptionsTitle = "数据异常警告";

class Smtp : public QObject
{
    Q_OBJECT
public:
    explicit Smtp(QString serverIP = "smtp.qq.com", QObject *parent = NULL);
    ~Smtp();
    void sendEmail(QString username, QString password, QString to, QString title, QString text, QString ip = "");

private:
    QString serverIP;
    QTcpSocket *tcpSocket;
    QString username;
    QString password;
    QString to;
    QString title;
    QString text;
    QByteArray serverText;
    int status;

signals:
    void disconnected();
    void emailStatus(int status);   //邮件发送的状态
    void progress(double p);

public slots:
    void connectToServer();
    void disconnectFromServer();
    void getMessage();

    void sendEmailSlot(QString message);

};

#endif // Smtp_H
