#include "GameMsg.h"


GameMsg::~GameMsg()
{

}

GameMsg::GameMsg(MSG_TYPE _msgtype, google::protobuf::Message* _pMsg):msgtype(_msgtype),pMsg(_pMsg)
{


}

GameMsg::GameMsg(MSG_TYPE _msgtype, std::string _stream)
{
	/*通过消息工厂创建消息对象*/
	switch (_msgtype)
	{
		case GameMsg::MSG_TYPE_LOGIN_ID_NAME:
		{
			pMsg = new pb::SyncPid();
			break;
		}
		case GameMsg::MSG_TYPE_CHAT_CONTENT:
		{
			pMsg = new pb::Talk();
			break;
		}
		case GameMsg::MSG_TYPE_NEW_PLACE:
		{
			pMsg = new pb::Position();
			break;
		}
		case GameMsg::MSG_TYPE_BROADCAST:
		{
			pMsg = new pb::BroadCast();
			break;
		}
		case GameMsg::MSG_TYPE_LOGOUT_ID_NAME:
		{
			pMsg = new pb::SyncPid();
			break;
		}
		case GameMsg::MSG_TYPE_OTHERPLAYERPLACE:
		{
			pMsg = new pb::SyncPlayers();
			break;
		}
		default:
		{
			break;
		}

	}
	/*将参数解析成对象内容  ba把_stream放入pmsg*/
	pMsg->ParseFromString(_stream);
}

std::string GameMsg::Serialize_msg()
{
	std::string res;

	pMsg->SerializePartialToString(&res);

	return res;
}
