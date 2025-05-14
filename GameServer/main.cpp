#include<zinx.h>
#include"GameChannel.h"


int main()
{
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