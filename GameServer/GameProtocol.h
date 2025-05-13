#pragma once
#include<zinx.h>
#include<iostream>
using namespace std;
/*协议层*/
class GameProtocol :
    public Iprotocol
{
    /*原始数据转化为目标数据*/
    UserData* raw2request(std::string _szInput) override;
    /*响应数据转化为原始数据*/
    std::string* response2raw(UserData& _oUserData) override;
    /*返回数据处理对象*/
    Irole* GetMsgProcessor(UserDataMsg& _oUserDataMsg) override;
    /*返回发送数据通道*/
    Ichannel* GetMsgSender(BytesMsg& _oBytes) override;
};

