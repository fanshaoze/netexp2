// server.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include "server.h"

BOOL ack[SEQ_SIZE+1];
 
int seq_now;//当前数据包的 seq
int WaitAck;//当前等待确认的 ack
int packetcount_total;//收到的包的总数
int needtosend_total;//需要发送的包总数


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
void getCurTime(char *ptime)
{
	char buffer[128];
	memset(buffer,0,sizeof(buffer));
	time_t c_time;
	struct tm *p;
	time(&c_time);
	p = localtime(&c_time);
	sprintf_s(buffer,"%d/%d/%d %d:%d:%d",
		p->tm_year + 1900,
		p->tm_mon,
		p->tm_mday,
		p->tm_hour,
		p->tm_min,
		p->tm_sec);
	strcpy_s(ptime,sizeof(buffer),buffer);
	/*
	struct tm {
        int tm_sec;     /* seconds after the minute - [0,59] 
        int tm_min;     /* minutes after the hour - [0,59] 
        int tm_hour;    /* hours since midnight - [0,23] 
        int tm_mday;    /* day of the month - [1,31] 
        int tm_mon;     /* months since January - [0,11] 
        int tm_year;    /* years since 1900 
        int tm_wday;    /* days since Sunday - [0,6] 
        int tm_yday;    /* days since January 1 - [0,365] 
        int tm_isdst;   /* daylight savings time flag 
        };
	*/
}
//************************************
// Method: seqIsAvailable
// FullName: seqIsAvailable
// Access: public
// Returns: bool
// Qualifier: 当前序列号 seq_now 是否可用
//************************************
bool seqIsAvailable()
{
	int windoswidth_now;
	windoswidth_now = seq_now - WaitAck;
	//cout << seq_now << WaitAck << endl;
	//cout << windoswidth_now << endl;
	if (seq_now-WaitAck < 0)
	{
		windoswidth_now +=  SEQ_SIZE;
	}
	if (windoswidth_now >= WINDOWS_WIDTH) return false;
	else if (ack[seq_now]) return true;
	else return false;
	/* 这里和书上理解上有一点点进化
	这个else if (ack[seq_now]) return true; 不太好理解，
	因为整个序列前后相连了，
	所以整个序列分成了三段，而不是书上写的四段，
	所有被确认过ack的其实都是可以用的
	*/
}
//************************************
// Method: timeoutHandler
// FullName: timeoutHandler
// Access: public
// Returns: void
// Qualifier: 超时重传处理函数，滑动窗口内的数据帧都要重传
//************************************
void timeoutHandler()
{
	int i;
	printf("Timer out error.\n");
	int index;
	for(i = 0 ; i < WINDOWS_WIDTH ; ++i)
	{
		if (i+WaitAck <= 20) index = i + WaitAck;
		else if (i+ WaitAck > 20) index = (i + WaitAck) % SEQ_SIZE;

		ack[index] = TRUE;
	}
	packetcount_total -= WINDOWS_WIDTH;
	seq_now = WaitAck;
}//************************************
// Method: ackHandler
// FullName: ackHandler
// Access: public
// Returns: void
// Qualifier: 收到 ack，累积确认，取数据帧的第一个字节
//由于发送数据时，第一个字节（序列号）为 0（ ASCII）时发送失败，因此加一了，此处需要减一还原
// Parameter: char c
//************************************
void ackHandler(char c)
{
	int i;
	unsigned char index = (unsigned char)c; 
	printf("Recv a ack of %d\n",index);
	if(WaitAck <= index)
	{
		for(i= WaitAck; i <= index;++i)	ack[i] = TRUE;//累计确认，当前的之前的ack全都收到了
		if ((index+1) <= 20) WaitAck = index+1;
		else if ((index+1) > 20) WaitAck = (index+1) % SEQ_SIZE;
	}
	else
	{
		//ack 超过了最大值，回到了 WaitAck 的左边
		for(i = WaitAck;i <= SEQ_SIZE ; ++i) ack[i] = TRUE;
		for(i = 1; i <= index ; ++i) ack[i] = TRUE;
		WaitAck = index + 1;
	}
}