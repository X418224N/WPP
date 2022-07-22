#ifndef PROTOCOL_H
#define PROTOCOL_H

#include<stdlib.h>
#include<unistd.h>
#include<string.h>


typedef unsigned int uint;


//#define REGIST_OK "regist sucess"
//#define REGIST_FAILED "regist failed : name existed"

//#define LOGIN_OK "login sucess"
//#define LOGIN_FAILED "login failed : name or pwd existed or online "

//#define SEARCH_USR_NO "no such people"
//#define SEARCH_USR_ONLINE "online"
//#define SEARCH_USR_OFFLINE "offline"

//#define UNKNOW_ERROR "unknow error"
//#define EXISTED_FRIEND "friend exist"
//#define ADD_FRIEND_OFFLINE "usr offline"
//#define ADD_FRIEND_NOEXIST "usr not exist"

//#define DEL_FRIEND_OK "delete friend ok"

//#define ROOTDIR_NO_EXIST "rootdir not exist"
//#define FILE_IS_EXIST "file is exist,please again"
//#define CREATE_DIR_OK "create dir ok"

//#define DEL_DIR_OK "delete dir is done"
//#define DEL_DIR_FAILURED "delete fail :is file"

//#define RENAME_FILE_OK "rename file ok"
//#define RENAME_FILE_FAILURED "rename file failured"

//#define ENTER_DIR_FAILURED "enter dir failured: is file"

//#define UPLOAD_FILE_OK "upload file ok"
//#define UPLOAD_FILE_FAILURED "upload file failured"

//#define DEL_FILE_OK "delete file is done"
//#define DEL_FILE_FAILURED "delete fail :is diretory"

//#define MOVE_FILE_OK "move file is done"
//#define MOVE_FILE_FAILURED "move fail :is file"
//#define COMMON_ERROE "move fail : system busy"
#define REGIST_OK "regist ok"
#define REGIST_FAILED "regist failed"

#define LOGIN_OK "login ok"
#define LOGIN_FAILED "login failed:name or pwd or relogin"

#define SEARCH_USR_NO "no such people"
#define SEARCH_USR_ONLINE "online"
#define SEARCH_USR_OFFLINE "offline"

#define DEL_FRIEND_OK "delete friend ok"
#define DIR_NO_EXIST "cur dir not exist"
#define FILE_NAME_EXIST "file name exist"
#define CREAT_DIR_OK "create dir ok"
#define RENAME_FILE_OK "rename file ok"
#define RENAME_FILE_FAILURED "rename file failured"
#define ENTER_DIR_FAILURED "ENTER DIR FAILURED:ISREGULER FILE"
#define REMAKE_OK "注销成功"
#define REMAKE_FAILURED "注销成功"
#define UNKNOW_ERROR "UNKNOW_ERROR"
#define EXISTED_FRIEND "EXISTED_FRIEND"
#define ADD_FRIEND_OFFLINE "ADD_FRIEND_OFFLINE"
#define ADD_FRIEND_NOEXIST "ADD_FRIEND_NOEXIST"
#define FILE_IS_EXIST "FILE_IS_EXIST"
#define ROOTDIR_NO_EXIST "ROOTDIR_NO_EXIST"
#define DEL_DIR_OK "DEL_DIR_OK"
#define DEL_DIR_FAILURED "DEL_DIR_FAILURED"
#define DEL_FILE_OK "DEL FILE OK"
#define DEL_FILE_FAILURED "DEL FAILURED OK"
#define MOVE_FILE_OK "MOVE FILE OK"
#define COMMON_ERROE "COMMON_ERROE"
#define MOVE_FILE_FAILURED "MOVE_FILE_FAILURED"
#define UPLOAD_FILE_OK "upload OK"
#define UPLOAD_FILE_FAILURED "upload failured"
enum ENUM_MSG_TYPE
{
    ENUM_MSG_TYPE_MIN=0,

    ENUM_MSG_TYPE_REGIST_REQUEST,   //注册请求
    ENUM_MSG_TYPE_REGIST_RESPOND,   //注册回复

    ENUM_MSG_TYPE_LOGIN_REQUEST,   //登入请求
    ENUM_MSG_TYPE_LOGIN_RESPOND,   //登入回复

    ENUM_MSG_TYPE_ALL_ONLINE_REQUEST,  //在线用户请求
    ENUM_MSG_TYPE_ALL_ONLINE_RESPOND,  //在线用户回复

    ENUM_MSG_TYPE_SEARCH_REQUEST,    //搜索用户请求
    ENUM_MSG_TYPE_SEARCH_RESPOND,   //搜索用户回复

    ENUM_MSG_TYPE_ADD_FRINED_REQUEST,  //添加好友请求
    ENUM_MSG_TYPE_ADD_FRINED_RESPOND,  //添加好友回复

    ENUM_MSG_TYPE_ADD_FRIEND_AGGREE,    //同意添加好友请求
    ENUM_MSG_TYPE_ADD_FRIEND_REFUSE,    //拒绝添加好友请求
    
    ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST,  //刷新好友请求
    ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND,  //刷新好友回复

    ENUM_MSG_TYPE_DEL_FRIEND_REQUEST,    //删除好友
    ENUM_MSG_TYPE_DEL_FRIEND_RESPOND,    //删除好友回复

    ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST,    //私聊请求
    ENUM_MSG_TYPE_PRIVATE_CHAT_RESPOND,    //私聊回复
    
    ENUM_MSG_TYPE_GROUP_CHAT_REQUEST,    //群聊请求
    ENUM_MSG_TYPE_GROUP_CHAT_RESPOND,    //群聊回复

    ENUM_MSG_TYPE_CREATE_DIR_REQUEST,    //创建文件夹请求
    ENUM_MSG_TYPE_CREATE_DIR_RESPOND,    //创建文件夹回复
    
    ENUM_MSG_TYPE_FLUSH_FILE_REQUEST,    //查看文件夹请求
    ENUM_MSG_TYPE_FLUSH_FILE_RESPOND,    //查看文件夹回复

    ENUM_MSG_TYPE_DEL_DIR_REQUEST,    //删除文件夹请求
    ENUM_MSG_TYPE_DEL_DIR_RESPOND,    //删除文件夹回复

    ENUM_MSG_TYPE_RENAME_REQUEST,    //重命名文件请求
    ENUM_MSG_TYPE_RENAME_RESPOND,    //重命名文件回复

    ENUM_MSG_TYPE_ENTER_DIR_REQUEST,    //进入文件请求
    ENUM_MSG_TYPE_ENTER_DIR_RESPOND,    //进入文件回复

    ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST,    //上传文件请求
    ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND,    //上传文件回复

    ENUM_MSG_TYPE_DEL_FILE_REQUEST,    //删除文件请求
    ENUM_MSG_TYPE_DEL_FILE_RESPOND,    //删除文件回复

    ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST,    //下载文件请求
    ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND,    //下载文件回复

    ENUM_MSG_TYPE_SHARE_FILE_REQUEST,    //共享文件请求
    ENUM_MSG_TYPE_SHARE_FILE_RESPOND,    //共享文件回复
    ENUM_MSG_TYPE_SHARE_FILE_NOTE,       //共享文件通知
    ENUM_MSG_TYPE_SHARE_FILE_NOTE_RESPOND,       //共享文件通知回复

    ENUM_MSG_TYPE_MOVE_FILE_REQUEST,    //移动文件请求
    ENUM_MSG_TYPE_MOVE_FILE_RESPOND,    //移动文件回复


    ENUM_MSG_TYPE_MAX=0x00ffffff



};


struct FileInfo
{
    char caFileName[32];   //文件名
    int iFileType;         //文件类型

};

struct PDU
{
    uint uiPDULen;  //总的协议数据单元
    uint uiMsgType; //消息类型
    char caData[64]; //文件名路径
    uint uiMsgLen;  //实际消息长度
    int caMsg[];    //实际消息
};

PDU * mkPDU(uint uiMsgLen);

#endif // PROTOCOL_H
