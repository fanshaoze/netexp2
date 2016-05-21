// net-exp2.cpp : 定义控制台应用程序的入口点。
//

#include "client.h"
#include "stdafx.h"

BOOL JudgeLoss(float packetLossRate)
{
	int loss = (int) (packetLossRate*100);
	int r = rand() % 101;
	if (r <= loss) return TRUE;
	else return FALSE;
}
int InitSock()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	//套接字加载时错误提示
	int err;
	//版本 2.2
	wVersionRequested = MAKEWORD(2, 2);
	//加载 dll 文件 Scoket 库
	err = WSAStartup(wVersionRequested, &wsaData);
	if(err != 0)
	{
		//找不到 winsock.dll
		printf("WSAStartup failed with error: %d\n", err);
	}
	if(LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) !=2)
	{
		printf("Could not find a usable version of Winsock.dll\n");
		WSACleanup();
	}
	else printf("The Winsock 2.2 dll was found okay\n");
	return err;
}
void printTips(){
	printf("-------------------------------------------\n");
	printf(" -help\n");
	printf(" -time : get current time \n");
	printf("| -quit : exit client \n");
	printf("| -testgbn : packetLossRate,ackLossRate : test the gbn \n");
	printf("-------------------------------------------\n");
}