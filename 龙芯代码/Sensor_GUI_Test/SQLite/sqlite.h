#ifndef SQLITE_H
#define SQLITE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>


class SQLiteManager
{
    Q_OBJECT
public:
    SQLiteManager();
    ~SQLiteManager();

    QString getUsername(){
        return this->username;
    }

    QString getPassword(){
        return this->password;
    }

    void openSQLite();
    void closeSQLite();
    void insertSQLite(QString username, QString password);
    void updateSQLite();

private:
    QSqlDatabase database;
    QSqlQuery sql_query;
    QString username = nullptr;
    QString password = nullptr;
    void sqlCreate();
    void createTable();
};

#endif // SQLITE_H
