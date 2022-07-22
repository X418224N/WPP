#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include<QTcpSocket>
#include"protocol.h"
#include"opendb.h"
#include<QDir>
#include<QFileInfoList>
#include<QFile>
#include<QTimer>


class MyTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    MyTcpSocket();
    QString getName();

    void copyDir(QString strSrcDir, QString strDestDir);
    
signals:
    void offline(MyTcpSocket *socket);
    
public slots:
    void recvMsg();
    void clientOff();
    void sendFileClient();
private:
    QString m_strName;

    QFile m_file;
    qint64 m_iTotal;
    qint64 m_iRecived;
    bool m_bUpload;

    QTimer *m_pTimer;
};

#endif // MYTCPSOCKET_H
