#include "GameProtocol.h"

UserData* GameProtocol::raw2request(std::string _szInput)
{
    cout << _szInput << endl;
    return nullptr;
}

std::string* GameProtocol::response2raw(UserData& _oUserData)
{
    return nullptr;
}

Irole* GameProtocol::GetMsgProcessor(UserDataMsg& _oUserDataMsg)
{
    return nullptr;
}

Ichannel* GameProtocol::GetMsgSender(BytesMsg& _oBytes)
{
    return nullptr;
}
