#ifndef OPENDB_H
#define OPENDB_H

#include <QObject>
#include<QSqlDatabase>
#include<QSqlQuery>
#include<QStringList>

class OpenDB : public QObject
{
    Q_OBJECT
public:
    explicit OpenDB(QObject *parent = nullptr);

       static OpenDB & getInstance();
       void init();
       ~OpenDB();

       bool handleRegist(const char *name, const char *pwd);
       bool handleLogin(const char *name, const char *pwd);
       void handleOffLine(const char *name);
       QStringList handleAllOnline();
       int handleSearchUsr(const char *name);
       int handleAddFriend(const char *pername, const char *loginname);
       void handleAgreeAddFriend(const char *pername,const char *loginname);
       QStringList handleFlushFriend(const char *name);
       bool handleDelFriend(const char *loginname,const char *friendname);
       

signals:

public slots:

private:
    QSqlDatabase m_db; //连接数据库
};

#endif // OPENDB_H
