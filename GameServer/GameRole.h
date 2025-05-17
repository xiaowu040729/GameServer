#pragma once
#include<zinx.h>
#include"AOI.h"
#include"GameMsg.h"



class GameProtocol;
/*业务层：用于对象处理业务*/
class GameRole :
    public Irole,Player
{
private:
    float x = 0;
    float y = 0;    /*高*/
    float z = 0;
    float v = 0;    /*视角*/
    int id = 0;
    string usrname;


public:
    GameRole();
    ~GameRole();
    /*要绑定的协议对象*/
    GameProtocol* protocol;

    bool Init() override;
    UserData* ProcMsg(UserData& _poUserData) override;
    void Fini() override;

    GameMsg* CreatLoginMsg();



    // 通过 Player 继承
    int GetX() override;
    int GetY() override;

    



};

