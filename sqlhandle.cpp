#include "sqlhandle.h"

sqlhandle::sqlhandle() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QCoreApplication::applicationDirPath() + "/Map.db");
    qDebug()<<QCoreApplication::applicationDirPath();
    if(!db.open()){
        qDebug() << "Error: Failed to open database:" << db.lastError().text();
        return;
    }
    QSqlQuery q(db);
    if (q.exec("SELECT name, type FROM sqlite_master WHERE type IN ('table','view') ORDER BY name;")) {
        qDebug() << "Tables in DB:";
        while (q.next()) {
            qDebug() << " -" << q.value(0).toString() << "(" << q.value(1).toString() << ")";
        }
    } else {
        qWarning() << "Failed to list tables:" << q.lastError().text();
    }
}
QString sqlhandle::JSONQuery(int ID){
    QSqlQuery query(db);
    qDebug()<<"SELECT * FROM Map WHERE ID="+QString::number(ID);
    if(query.exec("SELECT * FROM Map WHERE ID="+QString::number(ID))){
        qDebug()<<"SELECT * FROM Map WHERE ID="+QString::number(ID);
        if (!query.next()) { // 必须先移动到第一行
            qDebug() << "Query executed but no row with ID =" << ID;
            return {};
        }
        return query.value(0).toString();
    }else {
        qWarning() << "SQL error:" << query.lastError().text();
        return "";
    }
}
