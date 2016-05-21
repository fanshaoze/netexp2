// server.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include "stdafx.h"
#include "server.h"

BOOL ack[SEQ_SIZE+1];
 
int seq_now;//��ǰ���ݰ��� seq
int WaitAck;//��ǰ�ȴ�ȷ�ϵ� ack
int packetcount_total;//�յ��İ�������
int needtosend_total;//��Ҫ���͵İ�����


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
// Qualifier: ��ǰ���к� seq_now �Ƿ����
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
	/* ����������������һ������
	���else if (ack[seq_now]) return true; ��̫����⣬
	��Ϊ��������ǰ�������ˣ�
	�����������зֳ������Σ�����������д���ĶΣ�
	���б�ȷ�Ϲ�ack����ʵ���ǿ����õ�
	*/
}
//************************************
// Method: timeoutHandler
// FullName: timeoutHandler
// Access: public
// Returns: void
// Qualifier: ��ʱ�ش������������������ڵ�����֡��Ҫ�ش�
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
// Qualifier: �յ� ack���ۻ�ȷ�ϣ�ȡ����֡�ĵ�һ���ֽ�
//���ڷ�������ʱ����һ���ֽڣ����кţ�Ϊ 0�� ASCII��ʱ����ʧ�ܣ���˼�һ�ˣ��˴���Ҫ��һ��ԭ
// Parameter: char c
//************************************
void ackHandler(char c)
{
	int i;
	unsigned char index = (unsigned char)c; 
	printf("Recv a ack of %d\n",index);
	if(WaitAck <= index)
	{
		for(i= WaitAck; i <= index;++i)	ack[i] = TRUE;//�ۼ�ȷ�ϣ���ǰ��֮ǰ��ackȫ���յ���
		if ((index+1) <= 20) WaitAck = index+1;
		else if ((index+1) > 20) WaitAck = (index+1) % SEQ_SIZE;
	}
	else
	{
		//ack ���������ֵ���ص��� WaitAck �����
		for(i = WaitAck;i <= SEQ_SIZE ; ++i) ack[i] = TRUE;
		for(i = 1; i <= index ; ++i) ack[i] = TRUE;
		WaitAck = index + 1;
	}
}