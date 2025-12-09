#include "GameChannel.h"
#include"GameRole.h"


GameChannel::GameChannel(int _fd) : ZinxTcpData(_fd)
{

}

GameChannel::~GameChannel()
{
    if (NULL != iprotocol)
    {
        ZinxKernel::Zinx_Del_Proto(*iprotocol);
        delete iprotocol;
    }
}

AZinxHandler* GameChannel::GetInputNextStage(BytesMsg& _oInput)
{
    /*返回下一个处理对象*/
    return iprotocol;
}

ZinxTcpData* GameChannelFactory::CreateTcpDataChannel(int _fd)
{
    /*创建通道对象*/
    auto channel = new GameChannel(_fd);
    /*创建通道对象里的协议对象*/
    auto iprotocol = new GameProtocol();
    /*创建玩家对象*/
    auto irole = new GameRole();

    /*通道对象绑定协议对象*/
    channel->iprotocol = iprotocol;
    /*协议对象也绑定通道*/
    iprotocol->channel = channel;

    /*Role对象绑定协议对象*/
    irole->protocol = iprotocol;
    /*协议对象绑定Role对象*/
    iprotocol->role = irole;

    /*将协议对象加入框架中*/
    ZinxKernel::Zinx_Add_Proto(*iprotocol);

    /*将玩家对象加入框架中*/
    ZinxKernel::Zinx_Add_Role(*irole);

    return channel;
}
