#pragma once
#pragma once
#include <stdlib.h>
#include <time.h>
#include <WinSock2.h>
#include <fstream>
#pragma comment(lib,"ws2_32.lib")
#define SERVER_PORT 12340 //端口号
#define SERVER_IP "0.0.0.0" //IP 地址
const int BUFFER_LENGTH = 1026; //缓冲区大小，（以太网中 UDP 的数据帧中包长度应小于 1480 字节）
const int WINDOWS_WIDTH = 10;//发送窗口大小为 10， GBN 中应满足 W + 1 <=N（ W 为发送窗口大小， N 为序列号个数）
//本例取序列号 0...19 共 20 个
//如果将窗口大小设为 1，则为停-等协议
const int SEQ_SIZE = 20; //序列号的个数，从 1~20 共计 20 个
//由于发送数据第一个字节如果值为 0，则数据会发送失败
//因此接收端序列号为 1~20，与发送端一一对应
extern BOOL ack[SEQ_SIZE+1];//收到 ack 情况，对应 1~20 的 ack
extern int seq_now;//当前数据包的 seq
extern int WaitAck;//当前等待确认的 ack
extern int packetcount_total;//收到的包的总数
extern int needtosend_total;//需要发送的包总数

int InitSock();
void getCurTime(char *ptime);
bool seqIsAvailable();
void timeoutHandler();
void ackHandler(char c);