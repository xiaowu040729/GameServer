#include<zinx.h>
#include"GameChannel.h"
#include"GameMsg.h"
#include"msg.pb.h"
#include"AOI.h"
#include"NameGenerator.h"
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
	
	
	/*初始化姓名生成器（从Redis或文件加载姓名数据）*/
	NameGenerator* name_gen = NameGenerator::GetInstance();
	// 使用相对于可执行文件的路径，或者绝对路径
	// 假设可执行文件在GameServer目录，config在项目根目录
	std::string config_path = "../config";
	// 如果从项目根目录运行，使用相对路径
	// 如果从GameServer目录运行，使用../config
	if (!name_gen->Initialize(config_path)) {
		std::cerr << "姓名生成器初始化失败，将使用默认姓名" << std::endl;
		std::cerr << "尝试的路径: " << config_path << std::endl;
	}
	
	/*初始化框架*/
	ZinxKernel::ZinxKernelInit();
	/*添加监听通道，监听tcp请求*/
	ZinxKernel::Zinx_Add_Channel(*(new ZinxTCPListen(8899,new GameChannelFactory())));
	/*运行框架*/
	ZinxKernel::Zinx_Run();
	/*消除初始化*/
	ZinxKernel::ZinxKernelFini();
	
	/*清理姓名生成器*/
	NameGenerator::DestroyInstance();
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
