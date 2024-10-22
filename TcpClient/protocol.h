#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdlib.h>
#include <unistd.h>


/*消息类型是由枚举类型ENUM_MSG_TYPE来定义的，
 *需要在消息类型上进行更细致的区分可以PDU的消息描述部分（caMsgDsc）加以描述，
 *描述的工具就是宏
 *需要的空间：64b  */
#define RESPOND_TO_REGISTER_SUCCESSED "register success"
#define RESPOND_TO_REGISTER_FAILED "register failed : maybe name existed"
#define RESPOND_TO_LOGIN_SUCCESSED "login success"
#define RESPOND_TO_LOGIN_FAILED "register failed : incorrect username or password"
#define RESPOND_TO_LOGIN_REFUSED "register failed : You have logged in elsewhere"

#define RESPOND_TO_SEARCH_USR_ONLINE "usr is online"
#define RESPOND_TO_SEARCH_USR_OFFLINE "usr is offline"
#define RESPOND_TO_SEARCH_USR_ABSENT "usr is non-existent"

#define RESPOND_TO_ADDFRIEND_FAILED "usr is already your friend"
#define RESPOND_TO_ADDFRIEND_SENDED "The request has been sended"

#define RESPOND_TO_CREATE_DIR_SUCCESSED "Created successfully"
#define RESPOND_TO_CREATE_DIR_REPEATED "dir has been created before"
#define RESPOND_TO_CREATE_DIR_FAULT "path is not existent"
#define RESPOND_TO_CREATE_DIR_FAILED "unkown error"

#define RESPOND_TO_VIEW_DIR_SUCCESSED "success"
#define RESPOND_TO_VIEW_DIR_FAILED "failed"

#define RESPOND_TO_DEL_DIR_SUCCESSED "success"
#define RESPOND_TO_DEL_DIR_FAILED "failed"
#define RESPOND_TO_DEL_DIR_FAULT "path is not existent"

#define RESPOND_TO_RENAME_DIR_SUCCESSED "success"
#define RESPOND_TO_RENAME_DIR_FAILED "failed"


enum ENUM_MSG_TYPE{
    ENUM_MSG_TYPE_MIN = 0,
    ENUM_MSG_TYPE_REGISTER_REQUEST,
    ENUM_MSG_TYPE_REGISTER_RESPOND,//注册
    ENUM_MSG_TYPE_LOGIN_REQUEST,
    ENUM_MSG_TYPE_LOGIN_RESPOND,//登陆
    ENUM_MSG_TYPE_LOGOFF_REQUEST,
    ENUM_MSG_TYPE_LOGOFF_RESPOND,//注销


    ENUM_MSG_TYPE_SEARCH_USR_REQUEST,
    ENUM_MSG_TYPE_SEARCH_USR_RESPOND,//查询特定用户状态

    ENUM_MSG_TYPE_ADD_FRIEND_REQUEST,//加好友请求
    ENUM_MSG_TYPE_ADD_FRIEND_RESPOND,//好友申请是否发出
    //对方的回答
    ENUM_MSG_TYPE_ADD_FRIEND_AGREED,
    ENUM_MSG_TYPE_ADD_FRIEND_REFUSED,

    ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST,
    ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND,//刷新用户列表请求

    ENUM_MSG_TYPE_DEL_FRIEND_REQUEST,
    ENUM_MSG_TYPE_DEL_FRIEND_RESPOND,//删除用户请求

    ENUM_MSG_TYPE_PRIVATE_DATA,//私聊信息
    ENUM_MSG_TYPE_PRIVATE_FAILED,//发送失败

    ENUM_MSG_TYPE_CREATE_DIR_REQUEST,
    ENUM_MSG_TYPE_CREATE_DIR_RESPOND,//创建文件夹

    ENUM_MSG_TYPE_VIEW_DIR_REQUEST,
    ENUM_MSG_TYPE_VIEW_DIR_RESPOND,//查看文件夹

    ENUM_MSG_TYPE_DEL_DIR_REQUEST,
    ENUM_MSG_TYPE_DEL_DIR_RESPOND,//删除文件夹

    ENUM_MSG_TYPE_RENAME_DIR_REQUEST,
    ENUM_MSG_TYPE_RENAME_DIR_RESPOND,//重命名

    ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST = 50,//上传文件请求
    ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND,//已经做好接收文件的准备
    ENUM_MSG_TYPE_UPLOAD_FILE_DATA,//上传文件内容
    ENUM_MSG_TYPE_UPLOAD_FILE_SUC,//上传文件成功

    ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST,//下载文件请求
    ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND,//下载文件回复
    ENUM_MSG_TYPE_DOWNLOAD_FILE_START,//下载文件开始
    ENUM_MSG_TYPE_DOWNLOAD_FILE_DATA,//下载文件内容
    ENUM_MSG_TYPE_DOWNLOAD_FILE_SUC,//下载文件成功文件成功

    ENUM_MSG_TYPE_SHARE_FILE_REQUEST,//分享文件请求
    ENUM_MSG_TYPE_SHARE_FILE_RESPOND,//分享文件回复,回复了就是同意了

    ENUM_MSG_TYPE_CATALOG_REQUEST,
    ENUM_MSG_TYPE_CATALOG_DATA = 100,//目录信息，一个json文件
    //待添加
    ENUM_MSG_TYPE_MAX = 0xffffffff
};


struct PDU{
    unsigned int uiPDULen;      //协议数据单元总长度
    ENUM_MSG_TYPE enumMsgType;    //消息类型
    char caMsgDsc[64];          //消息描述
    unsigned int uiMsgLen;      //实际消息长度
    int caMsg[];                //实际消息
};

PDU *mkPDU(unsigned int uiMsgLen);

#endif // PROTOCOL_H
