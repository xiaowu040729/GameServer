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
    /*���Լ���������200��Ϣ*/
    auto pmsg = _pRole->SendPlayerToOthers();
    ZinxKernel::Zinx_SendOut(*pmsg, *protocol);

    /*�������ҷ��Լ���200��Ϣ*/
    pmsg = SendPlayerToOthers();
    ZinxKernel::Zinx_SendOut(*pmsg, *(_pRole->protocol));
}

void GameRole::viewDisappear(GameRole* _pRole)
{
    /*���Լ���������201��Ϣ*/
	auto pmsg = _pRole->CreateLogoutMsg();
	ZinxKernel::Zinx_SendOut(*pmsg, *protocol);

	/*�������ҷ��Լ���201��Ϣ*/
	pmsg = CreateLogoutMsg();
	ZinxKernel::Zinx_SendOut(*pmsg, *(_pRole->protocol));
}

GameRole::GameRole()
{
    /*����һ�£�������������Ϊtest*/
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
   /*������ҵĳ�ʼλ��*/
    /*生成玩家的随机初始位置*/
    // AOI世界范围: x: 0-400, y: 0-400
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> x_dist(0.0f, 400.0f);
    static std::uniform_real_distribution<float> z_dist(0.0f, 400.0f);
    static std::uniform_real_distribution<float> y_dist(0.0f, 50.0f);  // 高度范围0-50
    
    x = x_dist(gen);
    y = y_dist(gen);
    z = z_dist(gen);
    v = 0.0f;  // 初始速度
    
    std::cout << "[GameRole] 生成随机位置: x=" << x << ", y=" << y << ", z=" << z << std::endl;
    std::cout.flush();
    
    std::cout << "[GameRole] 生成随机位置: x=" << x << ", y=" << y << ", z=" << z << std::endl;
    std::cout.flush();
}

GameRole::~GameRole()
{

}

bool GameRole::Init()
{
    /*ͨ������ͨ����id����Ψһ��*/
    id = protocol->channel->GetFd();
    bool ret = false;

    /*������ҵ���Ϸ����*/
    ret = w.AddPlayer(this);

    if (true == ret)
    {
        /*�û�����ʱ��Ҫ���û�������Ϣ�� ID ���� ��*/
        GameMsg* loginmsg = CreatLoginMsg();
        ZinxKernel::Zinx_SendOut(*loginmsg, *protocol);

        /*�û����ߺ���Ҫ���䷢����Χ��ҵ���Ϣ*/
        loginmsg = SendOthersToPlayer();
        ZinxKernel::Zinx_SendOut(*loginmsg, *protocol);

        /*����Χ��ҷ��͸�����Լ���λ��*/
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
    /*��ʽ����Ϣ*/
    GET_REF2DATA(MultMsgs, input, _poUserData);
    /*ȡ����Ϣ*/
    for (auto single : input.msgs)
    {
        /*�����������Ϣ*/
        if (GameMsg::MSG_TYPE_CHAT_CONTENT == single->msgtype)
        {
            /*ȡ����������*/
            auto content = dynamic_cast<pb::Talk*> (single->pMsg)->content();
            /*ȡ����������������*/
            auto list_players = ZinxKernel::Zinx_GetAllRole();
            /*����ҷ�����Ϣ*/
            for (auto player : list_players)
            {
                auto role = dynamic_cast<GameRole*> (player);
                auto pmsg = SendBroadCast(content);
                ZinxKernel::Zinx_SendOut(*pmsg, *(role->protocol));
            }

        }

        /*���������ƶ���Ϣ*/
        if (GameMsg::MSG_TYPE_NEW_PLACE == single->msgtype)
        {
            /*��������ڵ�ͼ�����е�λ��*/
            auto NewPosition = dynamic_cast<pb::Position*> (single->pMsg);
            auto s1 = w.SurroundPlayers(this);
            /*ժ���ɸ���,��������,�����¸��ӣ���ȡ���ھ�s2*/
            w.DeletePlayer(this);
           
            x = NewPosition->x();
            y = NewPosition->y();
            z = NewPosition->z();
            v = NewPosition->v();
            w.AddPlayer(this);

            auto s2 = w.SurroundPlayers(this);
            /*����s2����Ԫ�ز�����s1, ��Ұ����*/
            for (auto single_player : s2)
            {
                if (s1.end() == find(s1.begin(), s1.end(), single_player))
                {
                    //��Ұ����
                    ViewAppear(dynamic_cast<GameRole*>(single_player));
                }
            }
            /*����s1����Ԫ�ز�����s2����Ұ��ʧ*/
            for (auto single_player : s1)
            {
                if (s2.end() == find(s2.begin(), s2.end(), single_player))
                {
                    //��Ұ��ʧ
                    viewDisappear(dynamic_cast<GameRole*>(single_player));
                }
            }

            /*������Χ��ҷ�����Ϣ*/
            auto player_list = w.SurroundPlayers(this);
            
            /*��������ҷ���������Ϣ*/
            for (auto player : player_list)
            {
                /*���Ҫ���͵ı���*/
                auto NewPosition = dynamic_cast<pb::Position*> (single->pMsg);
                auto broadmsg = new pb::BroadCast();
                auto setposition = broadmsg->mutable_p(); /*���ظ�����Ϣ��ָ��*/
                setposition->set_x(NewPosition->x());
                setposition->set_y(NewPosition->y());
                setposition->set_z(NewPosition->z());
                setposition->set_v(NewPosition->v());
                broadmsg->set_pid(id);
                broadmsg->set_username(usrname);
                broadmsg->set_tp(4);
                /*������Ϣ*/
                auto role = dynamic_cast<GameRole*> (player);
                GameMsg* pmsg = new GameMsg(GameMsg::MSG_TYPE_BROADCAST, broadmsg);
                ZinxKernel::Zinx_SendOut(*pmsg, *(role->protocol));
            }

        }


    }

    
    return nullptr;
}

/*�ڶ���ӿ����ժ����ʱ���ʹ��*/
void GameRole::Fini()
{
    /*����Χ��ҷ��͸���������ߵ���Ϣ*/
    auto srdplayer = w.SurroundPlayers(this);
    
    for (auto single : srdplayer)
    {
        auto role = dynamic_cast<GameRole*>(single);
       auto logoutmsg = CreateLogoutMsg();
        ZinxKernel::Zinx_SendOut(*logoutmsg, *(role->protocol));
    }

    w.DeletePlayer(this);
}

/*������ҵ�¼����Ϣ*/
GameMsg* GameRole::CreatLoginMsg()
{
    pb::SyncPid* pmsg = new pb::SyncPid();
    pmsg->set_pid(id);
    pmsg->set_username(usrname);
    GameMsg* gmsg = new GameMsg(GameMsg::MSG_TYPE_LOGIN_ID_NAME, pmsg);
    return gmsg;
}

/*���䷢����Χ��ҵ���Ϣ*/
GameMsg* GameRole::SendOthersToPlayer()
{
    /*��Χ����ǵ���Ϣ*/
    pb::SyncPlayers* surplayers_msg = new pb::SyncPlayers();

    /*�����Χ���*/
    auto surplayer = w.SurroundPlayers(this);
   
    for (auto single : surplayer)
    {
        /*������Χ��ҵ���Ϣ��surplayers��,������ָ������Ϣ��ָ��*/
        auto pPlayer = surplayers_msg->add_ps();
        /*single�������Ǹ������ͣ�������Ҫǿ��ת��������*/
        auto pRole = dynamic_cast<GameRole*>(single);
        
        /*��������Ϣ����Ϣ*/
        pPlayer->set_pid(pRole->id);
        pPlayer->set_username(pRole->usrname);
        /*����Ϣ����ʾ��Χ��ҵ�λ��*/
        /*������Ϣ�ҵ�����Ϣ�в���������Ϣ��λ��*/
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
    /*�㲥��Ϣ*/
    pb::BroadCast * pmsg = new pb::BroadCast();
    pmsg->set_pid(id);
    pmsg->set_username(usrname);

    /*�ͻ��˹涨 ���tp����Ϊ1��Ϊ������Ϣ tpΪ2��Ϊ��ʼλ����Ϣ*/
    pmsg->set_tp(2);

    /*���ø���Ϣ�������Ϣ*/
    auto position = pmsg->mutable_p();  
    position->set_x(x);
    position->set_y(y);
    position->set_z(z);
    position->set_v(v);
    GameMsg* res = new GameMsg(GameMsg::MSG_TYPE_BROADCAST,pmsg);
    return res;
}

/*������Ϣ*/
GameMsg* GameRole::CreateLogoutMsg()
{
    pb::SyncPid* pmsg = new pb::SyncPid();
    pmsg->set_pid(id);
    pmsg->set_username(usrname);
    GameMsg* pRet = new GameMsg(GameMsg::MSG_TYPE_LOGOUT_ID_NAME, pmsg);
    return pRet;
}

/*������Ϣ*/
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
