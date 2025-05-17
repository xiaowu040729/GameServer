#pragma once
#include<zinx.h>
#include<google/protobuf/message.h>
#include<list>
#include"msg.pb.h"
#include<iostream>
using namespace std;

/*不同层之间传递的消息写成一个类*/
/*一条消息*/
class GameMsg :
    public UserData
{
public:
   
    ~GameMsg();
    /*客户端的请求信息*/
    google::protobuf::Message* pMsg = NULL;
    /*消息ID：记录消息的类型*/
    enum MSG_TYPE {
        /*玩家登录ID*/
        MSG_TYPE_LOGIN_ID_NAME = 1,
        /*玩家聊天内容*/
        MSG_TYPE_CHAT_CONTENT = 2,
        /*玩家新位置*/
        MSG_TYPE_NEW_PLACE = 3,
        /*广播请求*/
        MSG_TYPE_BROADCAST = 200,
        /*下线玩家的ID*/
        MSG_TYPE_LOGOUT_ID_NAME = 201,
        /*周围玩家的位置*/
        MSG_TYPE_OTHERPLAYERPLACE = 202
    }msgtype;/*最后定义一个变量来存储*/

    /*已知消息内容创建消息对象*/
    GameMsg(MSG_TYPE _msgtype, google::protobuf::Message* _pMsg);
    /*已知一个字节流转换成消息*/
    GameMsg(MSG_TYPE _msgtype, std::string _stream);
    /*序列化是指将对象转换为便于存储或传输的格式*/
    /*这里是转化为字符串形式*/
    std::string Serialize_msg();

};

/*多条消息*/
class MultMsgs : public UserData {
public:
   std::list<GameMsg *> msgs;    
};

