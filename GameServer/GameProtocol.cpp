#include "GameProtocol.h"

/*返回转换后的消息对象MultMsg*/
UserData* GameProtocol::raw2request(std::string _szInput)
{
   MultMsgs* res = nullptr;
    /*前四个字节 四到八字节分别是 消息长度 消息ID*/
    msg.append(_szInput);
    /*不停的处理数据*/
    while (1)
    {
        /*如果此次传入的字节数<8那么就直接等下一次数据到达再处理*/
        if (msg.size() < 8)
        {
            break;
        }

        /*处理前四个字节*/
        int Byte_front_4 = 0;
        Byte_front_4 |= msg[0] << 0;
        Byte_front_4 |= msg[1] << 8;
        Byte_front_4 |= msg[2] << 16;
        Byte_front_4 |= msg[3] << 24;

        /*处理4到8个字节*/
        int Byte_4_to_8 = 0;
        Byte_4_to_8 |= msg[4] << 0;
        Byte_4_to_8 |= msg[5] << 8;
        Byte_4_to_8 |= msg[6] << 16;
        Byte_4_to_8 |= msg[7] << 24;

        /*判断后续报文长度是否合法*/
        if (msg.size() - 8 < Byte_front_4)
        {
            /*不合法就等待下一次操作*/
            break;
        }

        /*构造一条用户请求*/
        GameMsg* pmsg = new GameMsg((GameMsg::MSG_TYPE)Byte_4_to_8, msg.substr(8, Byte_front_4));
        res->msgs.push_back(pmsg);

        /*弹出以处理成功的报文*/
        msg.erase(0, 8+Byte_front_4);
            
        /*调试*/
        for (auto single : res->msgs)
        {
            cout << single->pMsg->Utf8DebugString() << endl;
        }
    }
    return res;
}

std::string* GameProtocol::response2raw(UserData& _oUserData)
{
    return nullptr;
}

Irole* GameProtocol::GetMsgProcessor(UserDataMsg& _oUserDataMsg)
{
    return nullptr;
}

Ichannel* GameProtocol::GetMsgSender(BytesMsg& _oBytes)
{
    return nullptr;
}
