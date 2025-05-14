#pragma once
#include<zinx.h>
#include<iostream>
#include"GameMsg.h"
#include<list>
#include"msg.pb.h"
using namespace std;
/*协议层*/
class GameProtocol :
    public Iprotocol
{
    /*缓存报文*/
    string msg;
public:
   
    /*原始数据转化为目标数据*/
    UserData* raw2request(std::string _szInput) override;
    /*响应数据转化为原始数据*/
    std::string* response2raw(UserData& _oUserData) override;
    /*返回数据处理对象*/
    Irole* GetMsgProcessor(UserDataMsg& _oUserDataMsg) override;
    /*返回发送数据通道*/
    Ichannel* GetMsgSender(BytesMsg& _oBytes) override;
};

