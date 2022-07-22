#include "mytcpsocket.h"
#include<QDebug>
#include<stdio.h>
#include"mytcpserver.h"
#include<QDir>


MyTcpSocket::MyTcpSocket()
{
    //connect(this,SIGNAL(readyRead()),this,SLOT(recvMsg()));
    connect(this,&MyTcpSocket::readyRead,[=](){

        this->recvMsg();

    });

    connect(this,&MyTcpSocket::disconnected,[=](){

        this->clientOff();

    });

    m_bUpload = false;
    m_pTimer = new QTimer;

    connect(m_pTimer,&QTimer::timeout,[=](){

        this->sendFileClient();
    });

}

QString MyTcpSocket::getName()
{
    return m_strName;
}

void MyTcpSocket::copyDir(QString strSrcDir, QString strDestDir)
{
    QDir dir;
    dir.mkdir(strDestDir);

    dir.setPath(strSrcDir);
    QFileInfoList fileInfoList = dir.entryInfoList();

    QString srcTmp;
    QString destTmp;
    for(int i=0; i<fileInfoList.size(); i++)
    {
        if(fileInfoList[i].isFile())
        {
            srcTmp = strSrcDir+'/'+fileInfoList[i].fileName();
            destTmp = strDestDir+'/'+fileInfoList[i].fileName();
            QFile::copy(srcTmp,destTmp);
        }
        else if(fileInfoList[i].isDir())
        {
            if(QString(".") == fileInfoList[i].fileName()
                || QString("..") == fileInfoList[i].fileName())
            {
                continue;
            }
            srcTmp = strSrcDir+'/'+fileInfoList[i].fileName();
            destTmp = strDestDir+'/'+fileInfoList[i].fileName();
            copyDir(srcTmp,destTmp);
        }

    }


}

void MyTcpSocket::recvMsg()
{
    if(!m_bUpload)
    {
        qDebug()<<this->bytesAvailable();
        uint uiPDULen = 0;
        this->read((char*)&uiPDULen,sizeof(uint));

        uint uiMsgLen = uiPDULen - sizeof(PDU);
        PDU * pdu = mkPDU(uiMsgLen);
        this->read((char*)pdu+sizeof(uint),uiPDULen - sizeof(uint));

        switch (pdu->uiMsgType)
        {
        case ENUM_MSG_TYPE_REGIST_REQUEST:
        {
            char caName[32] = {'\0'};
            char caPwd[32] = {'\0'};
            strncpy(caName,pdu->caData,32);
            strncpy(caPwd,pdu->caData+32,32);
            bool ret = OpenDB::getInstance().handleRegist(caName,caPwd);
            PDU *respdu = mkPDU(0);
            respdu->uiMsgType =ENUM_MSG_TYPE_REGIST_RESPOND;
            if(ret)
            {
                strcpy(respdu->caData,REGIST_OK);
                QDir dir;
                qDebug() << " create dir" <<dir.mkdir(QString("./%1").arg(caName));

            }
            else
            {
                strcpy(respdu->caData,REGIST_FAILED);
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;

            break;
        }
        case ENUM_MSG_TYPE_LOGIN_REQUEST:
        {
            char caName[32] = {'\0'};
            char caPwd[32] = {'\0'};
            strncpy(caName,pdu->caData,32);
            strncpy(caPwd,pdu->caData+32,32);
            bool ret = OpenDB::getInstance().handleLogin(caName,caPwd);

            PDU *respdu = mkPDU(0);
            respdu->uiMsgType =ENUM_MSG_TYPE_LOGIN_RESPOND;
            if(ret)
            {
                strcpy(respdu->caData,LOGIN_OK);
                m_strName = caName;
            }
            else
            {
                strcpy(respdu->caData,LOGIN_FAILED);
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;

            break;
        }
        case ENUM_MSG_TYPE_ALL_ONLINE_REQUEST:
        {
            QStringList ret = OpenDB::getInstance().handleAllOnline();
            uint uiMsgLen = ret.size()*32;
            PDU *respdu = mkPDU(uiMsgLen);
            respdu->uiMsgType = ENUM_MSG_TYPE_ALL_ONLINE_RESPOND;
            for(int i = 0;i < ret.size(); i++)
            {
                //memcpy((char*)(respdu->caMsg)+i*32, ret.at(i).toStdString.c_str(),ret.at(i).size());
                memcpy((char*)(respdu->caMsg)+i*32,
                       ret.at(i).toStdString().c_str(),
                       ret.at(i).size());
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;

            break;
        }
        case ENUM_MSG_TYPE_SEARCH_REQUEST:
        {
            int ret = OpenDB::getInstance().handleSearchUsr(pdu->caData);
            PDU * respdu = mkPDU(0);
            respdu->uiMsgType =ENUM_MSG_TYPE_SEARCH_RESPOND;
            if(-1 == ret)
            {
                strcpy(respdu->caData,SEARCH_USR_NO);
            }
            else if(1 == ret)
            {
                strcpy(respdu->caData,SEARCH_USR_ONLINE);
            }
            else if(0 == ret)
            {
                strcpy(respdu->caData,SEARCH_USR_OFFLINE);
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRINED_REQUEST:
        {
            char caPerName[32] = {'\0'};
            char caName[32] = {'\0'};
            strncpy(caPerName,pdu->caData,32);
            strncpy(caName,pdu->caData+32,32);
            int ret = OpenDB::getInstance().handleAddFriend(caPerName,caName);
            PDU * respdu = NULL;
            if(-1 == ret)
            {
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRINED_RESPOND;
                strcpy(respdu->caData,UNKNOW_ERROR);
                write((char*)respdu,respdu->uiPDULen);
                free(respdu);
                respdu = NULL;

            }
            else if(0 == ret)
            {
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRINED_RESPOND;
                strcpy(respdu->caData,EXISTED_FRIEND);
                write((char*)respdu,respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
            }
            else if(1 == ret)
            {
                MyTcpServer::getInstance().resend(caPerName,pdu);
            }
            else if(2 == ret)
            {
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRINED_RESPOND;
                strcpy(respdu->caData,ADD_FRIEND_OFFLINE);
                write((char*)respdu,respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
            }
            else if(3 == ret)
            {
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRINED_RESPOND;
                strcpy(respdu->caData,ADD_FRIEND_NOEXIST);
                write((char*)respdu,respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
            }
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_AGGREE:
        {
            char caPerName[32] = {'\0'};
            char caName[32] = {'\0'};
            strncpy(caPerName,pdu->caData,32);
            strncpy(caName,pdu->caData+32,32);
            OpenDB::getInstance().handleAgreeAddFriend(caPerName,caName);

            MyTcpServer::getInstance().resend(caName,pdu);
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_REFUSE:
        {
            char caName[32] ={'\0'};
            strncpy(caName,pdu->caData+32,32);
            MyTcpServer::getInstance().resend(caName,pdu);
            break;
        }
        case ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST:
        {
            char caName[32] = {'\0'};
            strncpy(caName,pdu->caData,32);
            QStringList ret = OpenDB::getInstance().handleFlushFriend(caName);
            uint uiMsgLen = ret.size()*32;
            PDU *respdu = mkPDU(uiMsgLen);
            respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND;
            for(int i =0; i < ret.size(); i++)
            {
                memcpy((char*)(respdu->caMsg)+i*32,ret.at(i).toStdString().c_str(),ret.at(i).size());
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
            //    case ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND:
            //    {
            //        char caName[32] ={'\0'};
            //        strncpy(caName,pdu->caData+32,32);
            //        QStringList ret = OpenDB::getInstance().handleFlushFriend();
            //    }
        case ENUM_MSG_TYPE_DEL_FRIEND_REQUEST:
        {
            char caSelfName[32] = {'\0'};
            char caFriendName[32] = {'\0'};
            strncpy(caSelfName,pdu->caData,32);
            strncpy(caFriendName,pdu->caData+32,32);
            OpenDB::getInstance().handleDelFriend(caSelfName,caFriendName);

            PDU *respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_DEL_FRIEND_RESPOND;
            strcpy(respdu->caData,DEL_FRIEND_OK);
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;

            MyTcpServer::getInstance().resend(caFriendName,pdu);

            break;
        }
        case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST:
        {
            char caPerName[32] = {'\0'};
            memcpy(caPerName,pdu->caData+32,32);

            qDebug() << caPerName;
            MyTcpServer::getInstance().resend(caPerName,pdu);
            break;
        }
        case ENUM_MSG_TYPE_GROUP_CHAT_REQUEST:
        {
            char caName[32] = {'\0'};
            strncpy(caName,pdu->caData,32);
            QStringList onlineFriend = OpenDB::getInstance().handleFlushFriend(caName);
            QString tmp;
            for(int i =0;i<onlineFriend.size();i++)
            {
                tmp = onlineFriend.at(i);
                MyTcpServer::getInstance().resend(tmp.toStdString().c_str(),pdu);
            }
            break;
        }
        case ENUM_MSG_TYPE_CREATE_DIR_REQUEST:
        {
            QDir dir;
            QString strCurPath = QString("%1").arg((char*)(pdu->caMsg));
            bool ret = dir.exists(strCurPath);
            PDU *respdu = mkPDU(0);
            if(ret)  //当前目录存在
            {
                char caNewDir[32] = {'\0'};
                memcpy(caNewDir,pdu->caData+32,32);
                QString strNewPath = strCurPath+"/"+caNewDir;
                //qDebug()<<strNewPath;
                ret = dir.exists(strNewPath);
                //qDebug()<< "===="<<ret;
                if(ret)  //创建的文件夹已存在
                {
                    respdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
                    strcpy(respdu->caData,FILE_IS_EXIST);
                }
                else     //创建的文件夹不存在，可以创建
                {
                    dir.mkdir(strNewPath);
                    respdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
                    strcpy(respdu->caData,CREAT_DIR_OK);
                }
            }
            else     //当前目录不存在
            {
                //qDebug()<<"没有宿主目录";
                respdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
                strcpy(respdu->caData,ROOTDIR_NO_EXIST);
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_FLUSH_FILE_REQUEST:
        {
            char *pCurPath = new char[pdu->uiMsgLen];
            qDebug()<<"hh";
            memcpy(pCurPath,pdu->caMsg,pdu->uiMsgLen);
            QDir dir(pCurPath);
            QFileInfoList fileInfoList = dir.entryInfoList();
            int iFileCount = fileInfoList.size();
            PDU *respdu = mkPDU(sizeof(FileInfo)*iFileCount);
            respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FILE_RESPOND;

            FileInfo *pFileInfo =NULL;
            QString strFileName;

            for(int i =0; i<iFileCount; i++)
            {
                pFileInfo = (FileInfo*)(respdu->caMsg)+i;
                strFileName = fileInfoList[i].fileName();

                memcpy(pFileInfo->caFileName,strFileName.toStdString().c_str(),strFileName.size());
                if(fileInfoList[i].isDir())
                {
                    pFileInfo->iFileType = 0;

                }
                else if(fileInfoList[i].isFile())
                {
                    pFileInfo->iFileType = 1;
                }

            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_DEL_DIR_REQUEST:
        {
            char caName[32] = {'\0'};
            strcpy(caName,pdu->caData);
            char *pPath =new char[pdu->uiMsgLen];
            memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);
            QString strPath =QString("%1/%2").arg(pPath).arg(caName);
            qDebug()<<strPath;

            QFileInfo fileInfo(strPath);
            bool ret =false;
            if(fileInfo.isDir())
            {
                QDir dir;
                dir.setPath(strPath);
                ret =dir.removeRecursively();
            }
            else if(fileInfo.isFile())
            {
                ret = false;
            }
            PDU *respdu = mkPDU(0);
            if(ret)
            {
                respdu->uiMsgType = ENUM_MSG_TYPE_DEL_DIR_RESPOND;
                strcpy(respdu->caData,DEL_DIR_OK);
            }
            else
            {
                respdu->uiMsgType = ENUM_MSG_TYPE_DEL_DIR_RESPOND;
                strcpy(respdu->caData,DEL_DIR_FAILURED);
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;

            break;


        }
        case ENUM_MSG_TYPE_RENAME_REQUEST:
        {
            char caOldName[32] = {'\0'};
            char caNewName[32] = {'\0'};
            strncpy(caOldName,pdu->caData,32);
            strncpy(caNewName,pdu->caData+32,32);

            char *pPath = new char[pdu->uiMsgLen];
            memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);


            QString strOldPath = QString("%1/%2").arg(pPath).arg(caOldName);
            QString strNewName = QString("%1/%2").arg(pPath).arg(caNewName);

            QDir dir;
            bool ret = dir.rename(strOldPath,strNewName);
            PDU *respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_RENAME_RESPOND;
            if(ret)
            {
                strcpy(respdu->caData,RENAME_FILE_OK);
            }
            else
            {
                strcpy(respdu->caData,RENAME_FILE_FAILURED);
            }

            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;


            break;
        }
        case ENUM_MSG_TYPE_ENTER_DIR_REQUEST:
        {
            char caEnterName[32] = {'\0'};
            strncpy(caEnterName,pdu->caData,32);


            char *pPath = new char[pdu->uiMsgLen];
            memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);

            QString strPath = QString("%1/%2").arg(pPath).arg(caEnterName);

            //qDebug()<<strPath;

            QFileInfo fileInfo(strPath);
            PDU *respdu =NULL;
            //PDU *respdu = mkPDU(sizeof(FileInfo)*iFileCount);

            if(fileInfo.isDir())
            {
                QDir dir(strPath);
                QFileInfoList fileInfoList = dir.entryInfoList();
                int iFileCount = fileInfoList.size();
                respdu = mkPDU(sizeof(FileInfo)*iFileCount);
                respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FILE_RESPOND;

                FileInfo *pFileInfo =NULL;
                QString strFileName;

                for(int i =0; i<iFileCount; i++)
                {
                    pFileInfo = (FileInfo*)(respdu->caMsg)+i;
                    strFileName = fileInfoList[i].fileName();

                    memcpy(pFileInfo->caFileName,strFileName.toStdString().c_str(),strFileName.size());
                    if(fileInfoList[i].isDir())
                    {
                        pFileInfo->iFileType = 0;

                    }
                    else if(fileInfoList[i].isFile())
                    {
                        pFileInfo->iFileType = 1;
                    }

                }
                write((char*)respdu,respdu->uiPDULen);
                free(respdu);
                respdu = NULL;


            }
            else if(fileInfo.isFile())
            {
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_ENTER_DIR_RESPOND;
                strcpy(respdu->caData,ENTER_DIR_FAILURED);

                write((char*)respdu,respdu->uiPDULen);
                free(respdu);
                respdu = NULL;


            }



            break;
        }
        case ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST:
        {
            char caFileName[32] = {'\0'};
            qint64 fileSize = 0;
            sscanf(pdu->caData,"%s %lld",caFileName,&fileSize);
            char *pPath =new char[pdu->uiMsgLen];
            memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);
            QString strPath =QString("%1/%2").arg(pPath).arg(caFileName);
            qDebug()<<strPath;
            delete []pPath;
            pPath = NULL;


            m_file.setFileName(strPath);
            //以只写的方式打开，若文件不存在，则会自动创建文件
            if(m_file.open(QIODevice::WriteOnly))
            {
                m_bUpload = true;
                m_iTotal = fileSize;
                m_iRecived =0;
            }
            break;
        }
        case ENUM_MSG_TYPE_DEL_FILE_REQUEST:
        {
            char caName[32] = {'\0'};
            strcpy(caName,pdu->caData);
            char *pPath =new char[pdu->uiMsgLen];
            memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);
            QString strPath =QString("%1/%2").arg(pPath).arg(caName);
            qDebug()<<strPath;



            QFileInfo fileInfo(strPath);
            bool ret =false;
            if(fileInfo.isDir())
            {
                ret = false;

            }
            else if(fileInfo.isFile())
            {
                QDir dir;
                //dir.setPath(strPath);
                ret =dir.remove(strPath);
            }
            PDU *respdu = mkPDU(0);
            if(ret)
            {
                respdu->uiMsgType = ENUM_MSG_TYPE_DEL_FILE_RESPOND;
                strcpy(respdu->caData,DEL_FILE_OK);
            }
            else
            {
                respdu->uiMsgType = ENUM_MSG_TYPE_DEL_FILE_RESPOND;
                strcpy(respdu->caData,DEL_FILE_FAILURED);
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;

            break;
        }
        case ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST:
        {
            char caFileName[32] = {'\0'};
            strcpy(caFileName,pdu->caData);
            char *pPath =new char[pdu->uiMsgLen];
            memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);
            QString strPath =QString("%1/%2").arg(pPath).arg(caFileName);
            qDebug()<<strPath;
            delete []pPath;
            pPath = NULL;

            QFileInfo fileInfo(strPath);
            qint64 fileSize = fileInfo.size();
            PDU *respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND;
            sprintf(respdu->caData,"%s %lld", caFileName,fileSize);

            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;

            m_file.setFileName(strPath);
            m_file.open(QIODevice::ReadOnly);
            m_pTimer->start(500);

            break;
        }
        case ENUM_MSG_TYPE_SHARE_FILE_REQUEST:
        {
            char caSendName[32] = {'\0'};
            int num = 0;
            sscanf(pdu->caData,"%s %d", caSendName, &num);
            int size = num * 32;
            PDU *respdu = mkPDU(pdu->uiMsgLen - size);
            respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_NOTE;
            strcpy(respdu->caData,caSendName);
            memcpy(respdu->caMsg,(char*)(pdu->caMsg)+size,pdu->uiMsgLen - size);

            char caRecvName[32] ={'\0'};
            for (int i=0; i<num; i++)
            {
                memcpy(caRecvName,(char*)(pdu->caMsg)+i*32,32);
                MyTcpServer::getInstance().resend(caRecvName,respdu);
            }
            free(respdu);
            respdu = NULL;

            respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_RESPOND;
            strcpy(respdu->caData,"share file ok");
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;


            break;
        }
        case ENUM_MSG_TYPE_SHARE_FILE_NOTE_RESPOND:
        {
            QString strRecvPath = QString("./%1").arg(pdu->caData);    //接收的目录
            QString strShareFilePath =QString("%1").arg((char*)(pdu->caMsg));    //发送的路径
            int index = strShareFilePath.lastIndexOf('/');
            QString strFileName = strShareFilePath.right(strShareFilePath.size()-index-1);
            strRecvPath = strRecvPath+'/'+strFileName;        //新的接收的路径

            QFileInfo fileInfo(strShareFilePath);
            if(fileInfo.isFile())
            {
                QFile::copy(strShareFilePath,strRecvPath);
            }
            else if(fileInfo.isDir())
            {
                copyDir(strShareFilePath,strRecvPath);
            }
        }
        case ENUM_MSG_TYPE_MOVE_FILE_REQUEST:
        {
            char caFileName[32] = {'\0'};
            int srcLen = 0;
            int destLen = 0;
            sscanf(pdu->caData, "%d%d%s", &srcLen, &destLen, caFileName);
            char *pSrcPath = new char[srcLen+1];
            char *pDestPath = new char[destLen+1+32];
            memset(pSrcPath, '\0', srcLen+1);
            memset(pDestPath, '\0', destLen+1+32);

            memcpy(pSrcPath,pdu->caMsg,srcLen);
            memcpy(pDestPath,(char*)(pdu->caMsg)+(srcLen+1),destLen);

            PDU *respdu =mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_MOVE_FILE_RESPOND;
            QFileInfo fileInfo(pDestPath);
            if(fileInfo.isDir())
            {
                strcat(pDestPath, "/");
                strcat(pDestPath, caFileName);

                bool ret = QFile::rename(pSrcPath, pDestPath);
                if(ret)
                {
                    strcpy(respdu->caData,MOVE_FILE_OK);
                }
                else
                {
                    strcpy(respdu->caData,COMMON_ERROE);
                }
            }
            else if(fileInfo.isFile())
            {
                strcpy(respdu->caData,MOVE_FILE_FAILURED);
            }

            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = NULL;

            break;
        }
        default:
            break;
        }


        free(pdu);
        pdu = NULL;
    }
    else
    {
        PDU * respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND;
        QByteArray buff =readAll();
        m_file.write(buff);
        m_iRecived += buff.size();
        if(m_iTotal == m_iRecived)
        {
            m_file.close();
            m_bUpload = false;
            strcpy(respdu->caData,UPLOAD_FILE_OK);

            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu =NULL;
        }
        else if(m_iTotal < m_iRecived && m_iTotal > m_iRecived )
        {
            m_file.close();
            m_bUpload = false;
            strcpy(respdu->caData,UPLOAD_FILE_FAILURED);

            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu =NULL;
        }

    }

    //qDebug() << pdu->uiMsgType <<(char*)(pdu->caMsg);

    //qDebug()<<caName<<caPwd<<pdu->uiMsgType;


}

void MyTcpSocket::clientOff()
{
    OpenDB::getInstance().handleOffLine(m_strName.toStdString().c_str());
    emit offline(this);
}

void MyTcpSocket::sendFileClient()
{
    m_pTimer->stop();
    char *pData = new char[4096];
    qint64 ret = 0;
    while (true)
    {
        ret = m_file.read(pData,4096);
        if(ret > 0 && ret <= 4096)
        {
            write(pData , ret);
        }
        else if(0 == ret)
        {
            m_file.close();
            break;
        }
        else if(ret < 0)
        {
            qDebug() << "发送文件给客户端失败";
            m_file.close();
            break;
        }
    }
    delete []pData;
    pData = NULL;
}
