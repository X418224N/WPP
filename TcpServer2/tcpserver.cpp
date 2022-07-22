#include "tcpserver.h"
#include "ui_tcpserver.h"


#include<QByteArray>
#include<QDebug>
#include<QMessageBox>
#include<QHostAddress>
#include<QFile>

TcpServer::TcpServer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TcpServer)
{
    ui->setupUi(this);

    loadConfig();
    MyTcpServer::getInstance().listen(QHostAddress(m_strIP),m_usPort);
}

TcpServer::~TcpServer()
{
    delete ui;
}

void TcpServer::loadConfig()
{
    QFile file(":/server.config");
      if(file.open(QIODevice::ReadOnly))
      {
          QByteArray bydata = file.readAll();
          QString strdata = bydata.toStdString().c_str();
          file.close();

          strdata.replace("\r\n"," ");

          QStringList strList = strdata.split(" ");

          m_strIP = strList.at(0);
          m_usPort = strList.at(1).toUShort();
          qDebug()<<"ip"<<m_strIP<<"port"<<m_usPort;

      }
      else
      {
          QMessageBox::critical(this,"oprn config","open config failed");
      }
}
