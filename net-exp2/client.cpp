// net-exp2.cpp : �������̨Ӧ�ó������ڵ㡣
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
	//�׽��ּ���ʱ������ʾ
	int err;
	//�汾 2.2
	wVersionRequested = MAKEWORD(2, 2);
	//���� dll �ļ� Scoket ��
	err = WSAStartup(wVersionRequested, &wsaData);
	if(err != 0)
	{
		//�Ҳ��� winsock.dll
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