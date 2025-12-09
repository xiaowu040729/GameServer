#include "GameRole.h"
#include"GameProtocol.h"
#include"msg.pb.h"
#include"GameChannel.h"
#include"NameGenerator.h"
#include <random>
#include <ctime>


static AOIWORLD w(0, 400, 0, 400, 20, 20);

void GameRole::ViewAppear(GameRole* _pRole)
{
    /*给自己发送对方的200消息*/
    auto pmsg = _pRole->SendPlayerToOthers();
    ZinxKernel::Zinx_SendOut(*pmsg, *protocol);

    /*给对方发送自己的200消息*/
    pmsg = SendPlayerToOthers();
    ZinxKernel::Zinx_SendOut(*pmsg, *(_pRole->protocol));
}

void GameRole::viewDisappear(GameRole* _pRole)
{
    /*给自己发送对方的201消息*/
	auto pmsg = _pRole->CreateLogoutMsg();
	ZinxKernel::Zinx_SendOut(*pmsg, *protocol);

	/*给对方发送自己的201消息*/
	pmsg = CreateLogoutMsg();
	ZinxKernel::Zinx_SendOut(*pmsg, *(_pRole->protocol));
}

GameRole::GameRole()
{
    /*初始化一下，将用户名设置为test（已废弃，现在使用随机姓名）*/
    /*通过Redis读取姓名文件生成随机姓名*/
    std::cout << "[GameRole] 开始创建GameRole，准备生成随机姓名..." << std::endl;
    std::cout.flush();
    
    NameGenerator* name_gen = NameGenerator::GetInstance();
    if (name_gen == nullptr) {
        std::cerr << "[GameRole] 错误: NameGenerator实例为空！" << std::endl;
        usrname = "test";
    } else {
        usrname = name_gen->GenerateRandomName();
        std::cout << "[GameRole] 生成的用户名: " << usrname << std::endl;
        std::cout.flush();
    }
   /*设置玩家的初始位置*/
    /*生成玩家的随机初始位置*/
    // AOI世界范围: x: 0-400, y: 0-400 (z用于2D网格的y坐标)
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> x_dist(200.0f, 300.0f); 
    static std::uniform_real_distribution<float> y_dist(200.0f, 300.0f); 
   
    x = 200.0f;
    y = 200.0f;
    z = 0.0f;
    v = 0.0f;

    // //随机位置
    // x = x_dist(gen);
    // y = y_dist(gen);  // y用于AOI系统的2D网格定位
    // z = 0.0f;  // z是高度，初始化为0（地面）
    // v = 0.0f;  // 初始速度
    
    std::cout << "[GameRole] 生成随机位置: x=" << x << ", y=" << y << ", z=" << z << std::endl;
    std::cout << "[GameRole] GetX()=" << GetX() << ", GetY()=" << GetY() << std::endl;
    std::cout.flush();
}

GameRole::~GameRole()
{

}


bool GameRole::Init()
{
    /*通过协议通道的id作为唯一标识*/
    id = protocol->channel->GetFd();
    bool ret = false;

    /*将玩家添加到游戏世界*/
    ret = w.AddPlayer(this);

    if (true == ret)
    {
        /*用户登录时需要给用户发送登录信息：ID 名字*/
        GameMsg* loginmsg = CreatLoginMsg();
        ZinxKernel::Zinx_SendOut(*loginmsg, *protocol);

        /*用户上线后需要给他发送周围玩家的信息*/
        loginmsg = SendOthersToPlayer();
        ZinxKernel::Zinx_SendOut(*loginmsg, *protocol);

        /*给周围玩家发送自己的位置*/
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
    /*获取消息*/
    for (auto single : input.msgs)
    {
        /*处理聊天内容消息*/
        if (GameMsg::MSG_TYPE_CHAT_CONTENT == single->msgtype)
        {
            /*获取聊天内容*/
            auto content = dynamic_cast<pb::Talk*> (single->pMsg)->content();
            /*获取所有在线玩家*/
            auto list_players = ZinxKernel::Zinx_GetAllRole();
            /*给玩家发送消息*/
            for (auto player : list_players)
            {
                auto role = dynamic_cast<GameRole*> (player);
                auto pmsg = SendBroadCast(content);
                ZinxKernel::Zinx_SendOut(*pmsg, *(role->protocol));
            }

        }

        /*处理玩家移动消息*/
        if (GameMsg::MSG_TYPE_NEW_PLACE == single->msgtype)
        {
            /*获取玩家在地图世界中的位置*/
            auto NewPosition = dynamic_cast<pb::Position*> (single->pMsg);
            auto s1 = w.SurroundPlayers(this);
            /*摘掉旧格子，更新位置，加入新格子，获取新邻居s2*/
            w.DeletePlayer(this);
           
            x = NewPosition->x();
            y = NewPosition->z();  // 客户端的z是水平坐标，作为服务器的y（AOI网格坐标）
            z = NewPosition->y();  // 客户端的y是高度，作为服务器的z
            v = NewPosition->v();
            
     
            
            w.AddPlayer(this);

            auto s2 = w.SurroundPlayers(this);
            /*遍历s2中元素不在s1中，视野出现*/
            for (auto single_player : s2)
            {
                if (s1.end() == find(s1.begin(), s1.end(), single_player))
                {
                    //视野出现
                    ViewAppear(dynamic_cast<GameRole*>(single_player));
                }
            }
            /*遍历s1中元素不在s2中，视野消失*/
            for (auto single_player : s1)
            {
                if (s2.end() == find(s2.begin(), s2.end(), single_player))
                {
                    //视野消失
                    viewDisappear(dynamic_cast<GameRole*>(single_player));
                }
            }

            /*给周围玩家发送位置信息*/
            auto player_list = w.SurroundPlayers(this);
            
            /*遍历周围玩家*/
            for (auto player : player_list)
            {
                /*获取要发送的位置*/
                /*注意：客户端期望的格式：x,z 是水平坐标，y 是高度
                 * 服务器内部存储：x,y 是2D网格坐标，z 是高度
                 * 所以发送时需要转换：使用服务器内部存储的值 */
                auto broadmsg = new pb::BroadCast();
                auto setposition = broadmsg->mutable_p(); /*设置位置消息的指针*/
                setposition->set_x(x);
                setposition->set_y(z);  // 发送高度（z）作为客户端的 y
                setposition->set_z(y);  // 发送水平坐标（y）作为客户端的 z
                setposition->set_v(v);
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

/*在通道接口摘除时会被使用*/
void GameRole::Fini()
{
    /*锟斤拷锟斤拷围锟斤拷曳锟斤拷透锟斤拷锟斤拷锟斤拷锟斤拷锟竭碉拷锟斤拷息*/
    auto srdplayer = w.SurroundPlayers(this);
    
    for (auto single : srdplayer)
    {
        auto role = dynamic_cast<GameRole*>(single);
       auto logoutmsg = CreateLogoutMsg();
        ZinxKernel::Zinx_SendOut(*logoutmsg, *(role->protocol));
    }

    w.DeletePlayer(this);
}

/*创建玩家登录消息*/
GameMsg* GameRole::CreatLoginMsg()
{
    pb::SyncPid* pmsg = new pb::SyncPid();
    pmsg->set_pid(id);
    pmsg->set_username(usrname);
    GameMsg* gmsg = new GameMsg(GameMsg::MSG_TYPE_LOGIN_ID_NAME, pmsg);
    return gmsg;
}

/*发送给周围玩家的信息*/
GameMsg* GameRole::SendOthersToPlayer()
{
    /*周围玩家的信息*/
    pb::SyncPlayers* surplayers_msg = new pb::SyncPlayers();

    /*获取周围玩家*/
    auto surplayer = w.SurroundPlayers(this);
   
    for (auto single : surplayer)
    {
        /*添加周围玩家的信息到surplayers中，添加指针指向消息的指针*/
        auto pPlayer = surplayers_msg->add_ps();
        /*single指向的是Player类型，所以需要强制转换为GameRole*/
        auto pRole = dynamic_cast<GameRole*>(single);
        
        /*设置玩家消息的信息*/
        pPlayer->set_pid(pRole->id);
        pPlayer->set_username(pRole->usrname);
        /*消息中显示周围玩家的位置*/
        /*注意：服务器内部使用 x,y 作为2D网格坐标，z 作为高度
         * 但客户端期望：x,z 是水平坐标，y 是高度
         * 所以发送时需要交换 y 和 z */
        auto position = pPlayer->mutable_p();
        position->set_x(pRole->x);
        position->set_y(pRole->z);  // 发送高度（z）作为客户端的 y
        position->set_z(pRole->y);  // 发送水平坐标（y）作为客户端的 z
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

    /*客户端规定 如果tp字段为1，为聊天消息 tp为2，为初始位置消息*/
    pmsg->set_tp(2);

    /*设置位置消息的位置消息*/
    /*注意：服务器内部使用 x,y 作为2D网格坐标，z 作为高度
     * 但客户端期望：x,z 是水平坐标，y 是高度
     * 所以发送时需要交换 y 和 z */
    auto position = pmsg->mutable_p();  
    position->set_x(x);
    position->set_y(z);  // 发送高度（z）作为客户端的 y
    position->set_z(y);  // 发送水平坐标（y）作为客户端的 z
    position->set_v(v);
    GameMsg* res = new GameMsg(GameMsg::MSG_TYPE_BROADCAST,pmsg);
    return res;
}

/*发送消息*/
GameMsg* GameRole::CreateLogoutMsg()
{
    pb::SyncPid* pmsg = new pb::SyncPid();
    pmsg->set_pid(id);
    pmsg->set_username(usrname);
    GameMsg* pRet = new GameMsg(GameMsg::MSG_TYPE_LOGOUT_ID_NAME, pmsg);
    return pRet;
}

/*发送消息*/
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
    return (int) y;
}
