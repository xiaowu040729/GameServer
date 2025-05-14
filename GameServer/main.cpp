#include<zinx.h>
#include"GameChannel.h"
#include"GameMsg.h"
#include"msg.pb.h"


int main()
{
	/*调试*/
	pb::SyncPid* pg = new pb::SyncPid();
	pg->set_pid(1);
	pg->set_username("test");
	GameMsg gm(GameMsg::MSG_TYPE::MSG_TYPE_LOGIN_ID_NAME, pg);
	auto output = gm.Serialize_msg();

	for (auto byte : output)
	{
		printf("%02X", byte);
	}

	puts("");

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