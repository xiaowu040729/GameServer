#pragma once
#include<ZinxTCP.h>
#include"GameProtocol.h"
/*tcp通道类：用于传送TCP数据*/

class GameChannel :
    public ZinxTcpData
{
public:
    GameChannel(int _fd);
    virtual ~GameChannel();
    /*在每个通道都绑定一个协议对象，以后该通道的信息都交给自己的协议对象处理*/
    GameProtocol* iprotocol =  nullptr;
    
    /* 返回一个协议对象*/
    AZinxHandler* GetInputNextStage(BytesMsg& _oInput) override;
};


/*创建channel对象*/
class GameChannelFactory : public IZinxTcpConnFact {
public:

    /*创建tcp通道*/
    ZinxTcpData* CreateTcpDataChannel(int _fd) override;

};