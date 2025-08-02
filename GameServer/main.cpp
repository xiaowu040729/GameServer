#include<zinx.h>
#include"GameChannel.h"
#include"GameMsg.h"
#include"msg.pb.h"
#include"AOI.h"
#include<iostream>



class MyPlayer : public Player {
	// 通过 Player 继承
public:
	MyPlayer(int _x, int _y,string _name) : x(_x), y(_y),name(_name) {};
	int GetX() override;
	int GetY() override;
	string GetName();
private:
	int x;
	int y;
	string name;
};
int main()
{
	///*调试AOI功能*/
	//{
	// 
	//	MyPlayer p1(60, 107, "1");
	//	MyPlayer p2(91, 118, "2");
	//	MyPlayer p3(147, 133, "3");

	//	w.AddPlayer(&p1);
	//	w.AddPlayer(&p2);
	//	w.AddPlayer(&p3);

	//	auto list_1 = w.SurroundPlayers(&p1);
	//	for (auto single : list_1)
	//	{
	//		dynamic_cast<MyPlayer*> (single);
	//		cout << dynamic_cast<MyPlayer*> (single)->GetName() << endl;
	//	}
	//}


	///*调试*/
	//{
	//	pb::SyncPid* pg = new pb::SyncPid();
	//	pg->set_pid(1);
	//	pg->set_username("test");
	//	GameMsg gm(GameMsg::MSG_TYPE::MSG_TYPE_LOGIN_ID_NAME, pg);
	//	auto output = gm.Serialize_msg();

	//	for (auto byte : output)
	//	{
	//		printf("%02X ", byte);
	//	}

	//	puts("");

	//	char buff[] = { 0x08, 0x01, 0x12, 0x04 ,0x74, 0x65, 0x73, 0x74 };
	//	std::string input(buff, sizeof(buff));

	//	auto ingm = GameMsg(GameMsg::MSG_TYPE_LOGIN_ID_NAME, input);
	//	std::cout << dynamic_cast<pb::SyncPid*> (ingm.pMsg)->pid() << std::endl;
	//	std::cout << dynamic_cast<pb::SyncPid*> (ingm.pMsg)->username() << std::endl;
	//}
	
	/*初始化框架*/
	ZinxKernel::ZinxKernelInit();
	/*添加监听通道，监听tcp请求*/
	ZinxKernel::Zinx_Add_Channel(*(new ZinxTCPListen(8899,new GameChannelFactory())));
	/*运行框架*/
	ZinxKernel::Zinx_Run();
	/*消除初始化*/
	ZinxKernel::ZinxKernelFini();
	return 0;
}

int MyPlayer::GetX()
{
	return x;
}

int MyPlayer::GetY()
{
	return y;
}

string MyPlayer::GetName()
{
	return name;
}
