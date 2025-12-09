#pragma once
#include<zinx.h>
#include"AOI.h"
#include"GameMsg.h"



class GameProtocol;
/*业务层：用于对象处理业务*/
class GameRole :
    public Irole,public Player
{
private:
    float x = 0;
    float y = 0;    /*高*/
    float z = 0;
    float v = 0;    /*视角*/
    int id = 0;
    string usrname;
    void ViewAppear(GameRole* _pRole);
    void viewDisappear(GameRole* _pRole);

public:
    GameRole();
    ~GameRole();
    /*要绑定的协议对象*/
    GameProtocol* protocol;

    bool Init() override;
    UserData* ProcMsg(UserData& _poUserData) override;
    void Fini() override;
    /*新客户连接后创建连接信息如ID Name等*/
    GameMsg* CreatLoginMsg();
    /*给其发送周围玩家的信息*/
    GameMsg* SendOthersToPlayer();
    /*向周围玩家发送自己的位置*/
    GameMsg* SendPlayerToOthers();
    /*玩家下线通知*/
    GameMsg* CreateLogoutMsg();
    /*发送聊天消息*/
    GameMsg* SendBroadCast(string content);


    // 通过 Player 继承
    int GetX() override;
    int GetY() override;

    
};

