#pragma once
#include <stdlib.h>
#include <WinSock2.h>
#include <time.h>
#pragma comment(lib,"ws2_32.lib")
#define	SERVER_PORT 12340
#define SERVER_IP "127.0.0.1"

const int buffer_length = 1026;
const int seq_size = 20;
BOOL JudgeLoss(float packetLossRate);
int InitSock();
void printTips();