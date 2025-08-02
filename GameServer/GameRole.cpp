#include "GameRole.h"
#include"GameProtocol.h"
#include"msg.pb.h"
#include"GameChannel.h"


static AOIWORLD w(0, 400, 0, 400, 20, 20);

void GameRole::ViewAppear(GameRole* _pRole)
{
    /*向自己发参数的200消息*/
    auto pmsg = _pRole->SendPlayerToOthers();
    ZinxKernel::Zinx_SendOut(*pmsg, *protocol);

    /*向参数玩家发自己的200消息*/
    pmsg = SendPlayerToOthers();
    ZinxKernel::Zinx_SendOut(*pmsg, *(_pRole->protocol));
}

void GameRole::viewDisappear(GameRole* _pRole)
{
    /*向自己发参数的201消息*/
	auto pmsg = _pRole->CreateLogoutMsg();
	ZinxKernel::Zinx_SendOut(*pmsg, *protocol);

	/*向参数玩家发自己的201消息*/
	pmsg = CreateLogoutMsg();
	ZinxKernel::Zinx_SendOut(*pmsg, *(_pRole->protocol));
}

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
    /*格式化消息*/
    GET_REF2DATA(MultMsgs, input, _poUserData);
    /*取出消息*/
    for (auto single : input.msgs)
    {
        /*如果是聊天消息*/
        if (GameMsg::MSG_TYPE_CHAT_CONTENT == single->msgtype)
        {
            /*取出聊天内容*/
            auto content = dynamic_cast<pb::Talk*> (single->pMsg)->content();
            /*取出世界里的所有玩家*/
            auto list_players = ZinxKernel::Zinx_GetAllRole();
            /*向玩家发送消息*/
            for (auto player : list_players)
            {
                auto role = dynamic_cast<GameRole*> (player);
                auto pmsg = SendBroadCast(content);
                ZinxKernel::Zinx_SendOut(*pmsg, *(role->protocol));
            }

        }

        /*如果是玩家移动消息*/
        if (GameMsg::MSG_TYPE_NEW_PLACE == single->msgtype)
        {
            /*更新玩家在地图网格中的位置*/
            auto NewPosition = dynamic_cast<pb::Position*> (single->pMsg);
            auto s1 = w.SurroundPlayers(this);
            /*摘出旧格子,更新坐标,添加新格子，获取新邻居s2*/
            w.DeletePlayer(this);
           
            x = NewPosition->x();
            y = NewPosition->y();
            z = NewPosition->z();
            v = NewPosition->v();
            w.AddPlayer(this);

            auto s2 = w.SurroundPlayers(this);
            /*遍历s2，若元素不属于s1, 视野出现*/
            for (auto single_player : s2)
            {
                if (s1.end() == find(s1.begin(), s1.end(), single_player))
                {
                    //视野出现
                    ViewAppear(dynamic_cast<GameRole*>(single_player));
                }
            }
            /*遍历s1，若元素不属于s2，视野消失*/
            for (auto single_player : s1)
            {
                if (s2.end() == find(s2.begin(), s2.end(), single_player))
                {
                    //视野消失
                    viewDisappear(dynamic_cast<GameRole*>(single_player));
                }
            }

            /*遍历周围玩家发送消息*/
            auto player_list = w.SurroundPlayers(this);
            
            /*向所有玩家发送聊天信息*/
            for (auto player : player_list)
            {
                /*组成要发送的报文*/
                auto NewPosition = dynamic_cast<pb::Position*> (single->pMsg);
                auto broadmsg = new pb::BroadCast();
                auto setposition = broadmsg->mutable_p(); /*返回复合消息的指针*/
                setposition->set_x(NewPosition->x());
                setposition->set_y(NewPosition->y());
                setposition->set_z(NewPosition->z());
                setposition->set_v(NewPosition->v());
                broadmsg->set_pid(id);
                broadmsg->set_username(usrname);
                broadmsg->set_tp(4);
                /*发送消息*/
                auto role = dynamic_cast<GameRole*> (player);
                GameMsg* pmsg = new GameMsg(GameMsg::MSG_TYPE_BROADCAST, broadmsg);
                ZinxKernel::Zinx_SendOut(*pmsg, *(role->protocol));
            }

        }


    }

    
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
       auto logoutmsg = CreateLogoutMsg();
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
    pb::SyncPid* pmsg = new pb::SyncPid();
    pmsg->set_pid(id);
    pmsg->set_username(usrname);
    GameMsg* pRet = new GameMsg(GameMsg::MSG_TYPE_LOGOUT_ID_NAME, pmsg);
    return pRet;
}

/*聊天消息*/
GameMsg* GameRole::SendBroadCast(string content)
{
    pb::BroadCast* pmsg = new pb::BroadCast();
    pmsg->set_pid(id);
    pmsg->set_username(usrname);
    pmsg->set_tp(1);
    pmsg->set_content(content);
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
