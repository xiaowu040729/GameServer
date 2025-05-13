#include "GameChannel.h"



GameChannel::GameChannel(int _fd) : ZinxTcpData(_fd)
{

}

GameChannel::~GameChannel()
{
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
    channel->iprotocol = new GameProtocol();
    /*将通道对象加入框架中*/
    ZinxKernel::Zinx_Add_Channel(*channel);
    return channel;
}
