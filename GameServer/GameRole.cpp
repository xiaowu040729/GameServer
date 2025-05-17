#include "GameRole.h"
#include"GameProtocol.h"
#include"msg.pb.h"
#include"GameChannel.h"


AOIWORLD w(0, 400, 0, 400, 6, 6);

GameRole::GameRole()
{
   
    /*测试一下，暂且设置名字为test*/
    usrname = "test";
}

GameRole::~GameRole()
{

}

bool GameRole::Init()
{
    id = protocol->channel->GetFd();
    bool ret = false;
    /*添加玩家到游戏世界*/
    ret = w.AddPlayer(this);

    /*用户上线时需要给用户发送消息如 ID 名字 等*/
    if (true == ret)
    {
        GameMsg* loginmsg = CreatLoginMsg();
        ZinxKernel::Zinx_SendOut(*loginmsg, *protocol);

    }
   
    return ret;
}

UserData* GameRole::ProcMsg(UserData& _poUserData)
{
    return nullptr;
}

void GameRole::Fini()
{
}

/*返回玩家登录的信息*/
GameMsg* GameRole::CreatLoginMsg()
{
    pb::SyncPid* pmsg = new pb::SyncPid();
    pmsg->set_pid(id);
    pmsg->set_username(usrname);
    GameMsg* gmsg = new GameMsg(GameMsg::MSG_TYPE_LOGIN_ID_NAME, pmsg);
    return gmsg;
}

int GameRole::GetX()
{
    return (int) x;
}

int GameRole::GetY()
{
    return (int) z;
}
