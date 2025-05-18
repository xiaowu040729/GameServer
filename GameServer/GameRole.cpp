#include "GameRole.h"
#include"GameProtocol.h"
#include"msg.pb.h"
#include"GameChannel.h"


static AOIWORLD w(0, 400, 0, 400, 20, 20);

GameRole::GameRole()
{
    /*测试一下，暂且设置名字为test*/
    usrname = "test";
   /*设置玩家的初始位置*/
    x = 100;
    z = 100;
}

GameRole::~GameRole()
{

}

bool GameRole::Init()
{
    /*通道创建通道的id具有唯一性*/
    id = protocol->channel->GetFd();
    bool ret = false;

    /*添加玩家到游戏世界*/
    ret = w.AddPlayer(this);

    if (true == ret)
    {
        /*用户上线时需要给用户发送消息如 ID 名字 等*/
        GameMsg* loginmsg = CreatLoginMsg();
        ZinxKernel::Zinx_SendOut(*loginmsg, *protocol);

        /*用户上线后还需要给其发送周围玩家的信息*/
        loginmsg = SendOthersToPlayer();
        ZinxKernel::Zinx_SendOut(*loginmsg, *protocol);

        /*向周围玩家发送该玩家自己的位置*/
        auto srdplayer = w.SurroundPlayers(this);
        for (auto single : srdplayer)
        {
            auto role = dynamic_cast<GameRole*>(single);
            loginmsg = SendPlayerToOthers();
            ZinxKernel::Zinx_SendOut(*loginmsg,*(role->protocol));
        }
    }
   
  
    return ret;
}

UserData* GameRole::ProcMsg(UserData& _poUserData)
{
    return nullptr;
}

/*在对象从框架里摘除的时候会使用*/
void GameRole::Fini()
{
    /*向周围玩家发送该玩玩家下线的消息*/
    auto srdplayer = w.SurroundPlayers(this);
    
    for (auto single : srdplayer)
    {
        auto role = dynamic_cast<GameRole*>(single);
       auto logoutmsg = CreatLoginMsg();
        ZinxKernel::Zinx_SendOut(*logoutmsg, *(role->protocol));
    }

    w.DeletePlayer(this);
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

/*给其发送周围玩家的信息*/
GameMsg* GameRole::SendOthersToPlayer()
{
    /*周围玩家们的信息*/
    pb::SyncPlayers* surplayers_msg = new pb::SyncPlayers();

    /*获得周围玩家*/
    auto surplayer = w.SurroundPlayers(this);
   
    for (auto single : surplayer)
    {
        /*添加周围玩家的信息到surplayers中,并返回指向子消息的指针*/
        auto pPlayer = surplayers_msg->add_ps();
        /*single的类型是父类类型，所以需要强制转换成子类*/
        auto pRole = dynamic_cast<GameRole*>(single);
        
        /*设置子消息的信息*/
        pPlayer->set_pid(pRole->id);
        pPlayer->set_username(pRole->usrname);
        /*子消息所表示周围玩家的位置*/
        /*把子消息挂到父消息中并返回子消息的位置*/
        auto position = pPlayer->mutable_p();
        position->set_x(pRole->x);
        position->set_y(pRole->y);
        position->set_z(pRole->z);
        position->set_v(pRole->v);
    }
    GameMsg* surmsg = new GameMsg(GameMsg::MSG_TYPE_OTHERPLAYERPLACE, surplayers_msg);
    return surmsg;
}

GameMsg* GameRole::SendPlayerToOthers()
{
    /*广播消息*/
    pb::BroadCast * pmsg = new pb::BroadCast();
    pmsg->set_pid(id);
    pmsg->set_username(usrname);

    /*客户端规定 如果tp设置为1则为聊天消息 tp为2则为初始位置消息*/
    pmsg->set_tp(2);

    /*设置父消息里的子消息*/
    auto position = pmsg->mutable_p();  
    position->set_x(x);
    position->set_y(y);
    position->set_z(z);
    position->set_v(v);
    GameMsg* res = new GameMsg(GameMsg::MSG_TYPE_BROADCAST,pmsg);
    return res;
}

/*下线消息*/
GameMsg* GameRole::CreateLogoutMsg()
{
    pb::BroadCast* pmsg = new pb::BroadCast();
    pmsg->set_pid(id);
    pmsg->set_username(usrname);    
    GameMsg* res = new GameMsg(GameMsg::MSG_TYPE_BROADCAST, pmsg);
    return res;
}



int GameRole::GetX()
{
    return (int) x;
}

int GameRole::GetY()
{
    return (int) z;
}
