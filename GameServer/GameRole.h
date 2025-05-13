#pragma once
#include<zinx.h>
/*业务层：用于对象处理业务*/
class GameRole :
    public Irole
{
    // 通过 Irole 继承
    bool Init() override;
    UserData* ProcMsg(UserData& _poUserData) override;
    void Fini() override;
};

