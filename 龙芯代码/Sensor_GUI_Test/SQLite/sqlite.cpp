#include "sqlite.h"

SQLiteManager::SQLiteManager()
{
    sqlCreate();
    createTable();
}

SQLiteManager::~SQLiteManager()
{
    closeSQLite();
}

void SQLiteManager::sqlCreate()
{
    if (QSqlDatabase::contains("User"))
    {
        database = QSqlDatabase::database("User");
    }
    else
    {
        database = QSqlDatabase::addDatabase("QSQLITE", "User");
        database.setDatabaseName("User.db");
        database.setUserName("lxh");
        database.setPassword("111111");
    }
}

void SQLiteManager::createTable()
{
    QString create_sql = "create table UserInfo (username varchar(10) primary key, password varchar(10))";
    if (!database.open())
    {
        qDebug() << "Error: Failed to connect database." << database.lastError();
    }
    else
    {
        sql_query.prepare(create_sql);
        if(!sql_query.exec())
        {
            qDebug() << "Error: Fail to create table." << sql_query.lastError();
        }
        else
        {
            qDebug() << "Table created!";
        }
    }
    database.close();
}

void SQLiteManager::openSQLite()
{

}

void SQLiteManager::closeSQLite()
{

}

void SQLiteManager::insertSQLite(QString username, QString password)
{
    QString insert_sql = "insert into UserInfo values (?, ?)";
    if (!database.open())
    {
        qDebug() << "Error: Failed to connect database." << database.lastError();
    }
    else
    {
        sql_query.prepare(insert_sql);
        sql_query.addBindValue(username);
        sql_query.addBindValue(password);
        if(!sql_query.exec())
        {
            qDebug() << sql_query.lastError();
        }
        else
        {
            qDebug() << "inserted Success!";
        }
    }
    database.close();
}

void SQLiteManager::querySQLite()
{
    QString select_all_sql = "select * from student";
    if (!database.open())
    {
        qDebug() << "Error: Failed to connect database." << database.lastError();
    }
    else
    {
        sql_query.prepare(select_all_sql);
        if(!sql_query.exec())
        {
            qDebug()<<sql_query.lastError();
        }
        else
        {
            if(sql_query.last()){
                do{
                    username = sql_query.value(0).toString();
                    password = sql_query.value(1).toString();
                }while(sql_query.next())
            }
        }
    }
    database.close();
}

