#pragma once
#include<zinx.h>


class GameProtocol;
/*业务层：用于对象处理业务*/
class GameRole :
    public Irole
{
private:
    


public:
    /*要绑定的协议对象*/
    GameProtocol* protocol;

    bool Init() override;
    UserData* ProcMsg(UserData& _poUserData) override;
    void Fini() override;
};

