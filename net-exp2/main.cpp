#include "stdafx.h"


int main(int argc, char* argv[])
{
	char command[128];
	int err;

	err = InitSock();
	SOCKET socketClient = socket(AF_INET, SOCK_DGRAM, 0);
	SOCKADDR_IN addrServer;
	addrServer.sin_addr.S_un.S_addr = inet_addr(SERVER_IP);
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(SERVER_PORT);
	//接收缓冲区
	char buffer[buffer_length]; 
	ZeroMemory(buffer,sizeof(buffer));
	int len = sizeof(SOCKADDR);
	//为了测试与服务器的连接，可以使用 -time 命令从服务器端获得当前时间
	//使用 -testgbn [X] [Y] 测试 GBN 其中[X]表示数据包丢失概率
	// [Y]表示 ACK 丢包概率 
	printTips();
	int ret;
	int interval = 1;//收到数据包之后返回 ack 的间隔，默认为 1 表示每个都返回 ack， 0 或者负数均表示所有的都不返回 ack
	float packetLossRate = 0.2; //默认包丢失率 0.2
	float ackLossRate = 0.2; //默认 ACK 丢失率 0.2

	//用时间作为随机种子，放在循环的最外面
	srand((unsigned)time(NULL));
	while(true)
	{
		gets_s(buffer);
		ret = sscanf(buffer,"%s%f%f",&command,&packetLossRate,&ackLossRate);
		/*
		函数原型:
		int sscanf( const char *, const char *, ...);
		int sscanf(const char *buffer,const char *format,[argument ]...);
		buffer存储的数据
		format格式控制字符串
		argument 选择性设定字符串
		sscanf会从buffer里读进数据，依照format的格式将数据写入到argument里。*/
		//开始 GBN 测试，使用 GBN 协议实现 UDP 可靠文件传输
		if(!strcmp(command,"-testgbn"))
		{
			cout<<"Begin to test GBN protocol, please don't abort theprocess"<<endl;
			printf("The loss ratio of packet is %.2f,the loss ratio of ack is %.2f\n",packetLossRate,ackLossRate);
			int waitCount = 0;
			int stage = 0;
			BOOL b;
			unsigned char state_code;//状态码
			unsigned short packet_number;//包的序列号
			unsigned short recv_number;//接收窗口大小为 1，已确认的序列号
			unsigned short expect_number;//等待的序列号
			sendto(socketClient, "-testgbn", strlen("-testgbn")+1, 0, (SOCKADDR*)&addrServer, sizeof(SOCKADDR));
			while (true)
			{
				//等待 server 回复设置 UDP 为阻塞模式
				recvfrom(socketClient,buffer,buffer_length,0,(SOCKADDR*)&addrServer, &len);
				//cout<<buffer<<endl;
				switch(stage)
				{
					case 0://等待握手阶段
						state_code = (unsigned char)buffer[0];
						if ((unsigned char)buffer[0] == 205)
						{
							cout<<"Ready for file transmission"<<endl;
							buffer[0] = 200;
							buffer[1] = '\0';
							sendto(socketClient, buffer, 2, 0,(SOCKADDR*)&addrServer, sizeof(SOCKADDR));
							stage = 1;
							recv_number = 0;
							expect_number = 1;
						}
						break;
					case 1://等待接收数据阶段
						packet_number = (unsigned short)buffer[0];
						b = JudgeLoss(packetLossRate);
						if(b)
						{
							cout<<"The packet with a packet_number of "<<(unsigned short)packet_number<<" loss"<<endl;
							continue;
						}
						cout<<"recv a packet with a packet_number of "<<(unsigned short)packet_number<<endl;
						//如果是期待的包，正确接收，正常确认即可
						if(expect_number == packet_number)
						{
							++expect_number;
							if(expect_number == 21)	expect_number = 1;
							//输出数据
							//cout<<&buffer[1]<<endl;
							buffer[0] = packet_number;
							recv_number = packet_number;
							buffer[1] = '\0';
						}
						else
						{
						
							if(recv_number == 0) continue;
							else 
							{
								buffer[0] = recv_number;//上一次的那个ack的值
								buffer[1] = '\0';
							}
						}
						//ack同样也是概率的丢包!!!!!!!!!
						b = JudgeLoss(ackLossRate);
						int ack_num;
						ack_num =(unsigned char)buffer[0]+0;
						if(b)
						{
							printf("The ack of %d loss\n", (unsigned char)buffer[0]);
							//cout<<"The ack of "<< ack_num <<" loss"<<endl;
							continue;
						}
						else 
						{
							sendto(socketClient, buffer, 2, 0,(SOCKADDR*)&addrServer, sizeof(SOCKADDR));
							printf("send a ack of %d\n", (unsigned char)buffer[0]);
							//cout<<"send a ack of "<< ack_num <<endl;
							break;
						}
				}
				Sleep(500);
			}
		}
		//GBN结束,
		sendto(socketClient, buffer, strlen(buffer)+1, 0,(SOCKADDR*)&addrServer, sizeof(SOCKADDR));
		ret = recvfrom(socketClient,buffer,buffer_length,0,(SOCKADDR*)&addrServer,&len);
		cout<<buffer<<endl;
		if(!strcmp(buffer,"Good bye!")) break;
		else printTips();
	}
	//关闭套接字
	closesocket(socketClient);
	WSACleanup();
	return 0;
}

