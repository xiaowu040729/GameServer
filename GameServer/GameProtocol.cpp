#include "GameProtocol.h"
#include"GameChannel.h"
#include"GameRole.h"
GameProtocol::GameProtocol()
{
}

GameProtocol::~GameProtocol()
{
}

/*返回转换后的消息对象MultMsg*/
UserData* GameProtocol::raw2request(std::string _szInput)
{
   MultMsgs* res =  new MultMsgs();
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
        msg.erase(0, 8 + Byte_front_4);
            
        /*调试*/
        for (auto single : res->msgs)
        {   
            cout << single->pMsg->Utf8DebugString() << endl;
        }
    }

    /*调试：接收到消息的同时发送一条信息*/
    pb::Talk* pt = new pb::Talk();
    pt->set_content("hello");
    GameMsg* tmsg = new GameMsg(GameMsg::MSG_TYPE_CHAT_CONTENT, pt);
    ZinxKernel::Zinx_SendOut(*(tmsg), *this);

    return res;
}


/*参数来自业务层，待发送的数据，转化成字符串*/
std::string* GameProtocol::response2raw(UserData& _oUserData)
{

    string *rstring = new string();
    /*要发送的报文组成*/
    int length = 0;
    int id = 0;
    string rmsg;
    GET_REF2DATA(GameMsg, output, _oUserData);
    
    /*初始化报文*/
    id = output.msgtype;
    rmsg = output.Serialize_msg();
    length = rmsg.size();

    /*前4个字节放长度信息,低字节在前高字节在后*/
    rstring->push_back((length >> 0) & 0xff);
    rstring->push_back((length >> 8) & 0xff);
    rstring->push_back((length >> 16) & 0xff);
    rstring->push_back((length >> 24) & 0xff);

    /*处理第5个字节到第8个字节*/
    rstring->push_back((id >> 0) & 0xff);
    rstring->push_back((id >> 8) & 0xff);
    rstring->push_back((id >> 16) & 0xff);
    rstring->push_back((id >> 24) & 0xff);

    /*拼接第7个字节以后的数据*/
    rstring->append(rmsg);


    return rstring;
}


Irole* GameProtocol::GetMsgProcessor(UserDataMsg& _oUserDataMsg)
{
    /*处理完的数据再返回给对象*/
    return role;
}

/*返回数据发送的通道*/
Ichannel* GameProtocol::GetMsgSender(BytesMsg& _oBytes)
{
    /*返回通道对象*/
    return channel;
}
