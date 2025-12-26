#ifndef SQLHANDLE_H
#define SQLHANDLE_H
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlTableModel>
#include<QDebug>
#include <QCoreApplication>
#include<QFile>
class sqlhandle
{
public:
    sqlhandle();
    QString JSONQuery(int ID);
private:
    QSqlDatabase db;
};

#endif // SQLHANDLE_H
