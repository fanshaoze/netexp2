#pragma once
#pragma once
#include <stdlib.h>
#include <time.h>
#include <WinSock2.h>
#include <fstream>
#pragma comment(lib,"ws2_32.lib")
#define SERVER_PORT 12340 //�˿ں�
#define SERVER_IP "0.0.0.0" //IP ��ַ
const int BUFFER_LENGTH = 1026; //��������С������̫���� UDP ������֡�а�����ӦС�� 1480 �ֽڣ�
const int WINDOWS_WIDTH = 10;//���ʹ��ڴ�СΪ 10�� GBN ��Ӧ���� W + 1 <=N�� W Ϊ���ʹ��ڴ�С�� N Ϊ���кŸ�����
//����ȡ���к� 0...19 �� 20 ��
//��������ڴ�С��Ϊ 1����Ϊͣ-��Э��
const int SEQ_SIZE = 20; //���кŵĸ������� 1~20 ���� 20 ��
//���ڷ������ݵ�һ���ֽ����ֵΪ 0�������ݻᷢ��ʧ��
//��˽��ն����к�Ϊ 1~20���뷢�Ͷ�һһ��Ӧ
extern BOOL ack[SEQ_SIZE+1];//�յ� ack �������Ӧ 1~20 �� ack
extern int seq_now;//��ǰ���ݰ��� seq
extern int WaitAck;//��ǰ�ȴ�ȷ�ϵ� ack
extern int packetcount_total;//�յ��İ�������
extern int needtosend_total;//��Ҫ���͵İ�����

int InitSock();
void getCurTime(char *ptime);
bool seqIsAvailable();
void timeoutHandler();
void ackHandler(char c);