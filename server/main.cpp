// server.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

//主函数
int main(int argc, char* argv[])
{
	int size;
	int err;
	//加载套接字库（必须） 
	err = InitSock();
	SOCKET sockServer = socket(AF_INET, SOCK_DGRAM,IPPROTO_UDP);
	//地址格式，udp，udp协议
	//设置套接字为非阻塞模式
	int iMode = 1; //1：非阻塞， 0：阻塞
	ioctlsocket(sockServer,FIONBIO, (u_long FAR*) &iMode);//非阻塞设置
	//#define FIONBIO     _IOW('f', 126, u_long) /* set/clear non-blocking i/o */
	/*int ioctlsocket( int s, long cmd, u_long * argp);
	s：一个标识套接口的描述字。
	cmd：对套接口s的操作命令。
	argp：指向cmd命令所带参数的指针。
	注释：编辑本函数可用于任一状态的任一套接口。它用于获取与套接口相关的操作参数，而与具体协议或通讯子系统无关。支持下列命令：
	*/
	SOCKADDR_IN addrServer; //服务器地址
	//addrServer.sin_addr.S_un.S_addr = inet_addr(SERVER_IP);
	addrServer.sin_addr.S_un.S_addr = htonl(INADDR_ANY);//两者均可
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(SERVER_PORT);
	err = bind(sockServer,(SOCKADDR*)&addrServer, sizeof(SOCKADDR));
	//bind理解为给socket结构里面的Ip地址和端口号赋值
	if(err)
	{
		err = GetLastError();//检测bind函数有没有正确执行
		printf("Could not bind the port %d for socket.Error codeis %d\n",SERVER_PORT,err);

		WSACleanup();
		return -1;
	}
	SOCKADDR_IN addrClient; //客户端地址
	int length = sizeof(SOCKADDR);
	char buffer[BUFFER_LENGTH]; //数据发送接收缓冲区
	ZeroMemory(buffer,sizeof(buffer));
	FILE* file = fopen("ReadMe.txt", "rb");
	if (file)
	{
		size = _filelength(_fileno(file));
		cout << size << endl;
		fclose(file);
	}
	//将测试数据读入内存
	std::ifstream icin;
	//ifstream file2("c:\\pdos.def");以输入方式打开文件
	icin.open("test.txt");//?????测试文件？？？？
	char *data = new char[size];
	ZeroMemory(data,sizeof(data));
	icin.read(data,size);
	//　read()从文件中读取 num 个字符到 buf 指向的缓存中，
	//http://www.cppblog.com/saga/archive/2007/06/19/26652.html fstream的具体函数的功能和用法
	icin.close();
	packetcount_total = size / 1024;
	int recvSize ;
	for(int i=1; i <= SEQ_SIZE; ++i)	ack[i] = TRUE;
	while(true)
	{
		//非阻塞接收，若没有收到数据，返回值为-1
		recvSize = recvfrom(sockServer,buffer,BUFFER_LENGTH,0,((SOCKADDR*)&addrClient),&length);
		if(recvSize < 0)
		{
			Sleep(200);
			continue;
		}
		printf("recv from client: %s\n",buffer);
		if(strcmp(buffer,"-time") == 0)
		{
			getCurTime(buffer);
		}
		else if(strcmp(buffer,"-quit") == 0)
		{
			strcpy_s(buffer,strlen("Good bye!") + 1,"Good bye!");
		}
		else if(strcmp(buffer,"-testgbn") == 0)
		{
			//进入 gbn 测试阶段
			//首先 server（ server 处于 0 状态）向 client 发送 205 状态码（ server进入 1 状态）

			//server 等待 client 回复 200 状态码，如果收到（ server 进入 2 状态），则开始传输文件，否则延时等待直至超时\
			//在文件传输阶段， server 发送窗口大小设为
			ZeroMemory(buffer,sizeof(buffer));
			int recvSize;
			int counter = 0;
			cout<<"Begain to test GBN protocol"<<endl;
			//加入了一个握手阶段
			//首先服务器向客户端发送一个 205 大小的状态码（我自己定义的）表示服务器准备好了，可以发送数据
			//客户端收到 205 之后回复一个 200 大小的状态码，表示客户端准备好了，可以接收数据了
			//服务器收到 200 状态码之后，就开始使用 GBN 发送数据了
			cout<<"Shake hands stage"<<endl;
			int stage = 0;
			bool runFlag = true;
			while(runFlag)
			{
				switch(stage)
				{
					case 0://发送 205 阶段
						buffer[0] = 205;
						sendto(sockServer, buffer, strlen(buffer)+1, 0,(SOCKADDR*)&addrClient, sizeof(SOCKADDR));
						/*
						s 套接字
						buff 待发送数据的缓冲区
						size 缓冲区长度
						Flags 调用方式标志位, 一般为0, 改变Flags，将会改变Sendto发送的形式
						addr （可选）指针，指向目的套接字的地址
						len addr所指地址的长度
						*/
						Sleep(100);
						stage = 1;
						break;
					case 1://等待接收 200 阶段，没有收到则计数器+1，超时则放弃此次“连接”，等待从第一步开始
						recvSize = recvfrom(sockServer,buffer,BUFFER_LENGTH,0,((SOCKADDR*)&addrClient),&length);
						if(recvSize < 0)
						{
							++counter;
							if(counter > 20)
							{
								runFlag = false;
								cout<<"Timeout"<<endl;
								break;
							}
							Sleep(500);
							continue;
						}
						else
						{
							if((unsigned char)buffer[0] == 200)
							{
								cout<<"Begin a file transfer"<<endl;
								printf("File size is %dB, each packet is 1024B and packet total num is %d\n",size,needtosend_total);
								seq_now = 1;
								WaitAck = 1;
								packetcount_total = 0;
								counter = 0;
								stage = 2;
							}
						}
						break;
					case 2://数据传输阶段，发送给客户端的序列号从 1 开始
						if(seqIsAvailable())
						{
						
							buffer[0] = seq_now;
							ack[seq_now] = FALSE;
							//在开始的时候，要把当前的这个数据的位置置为false，也就是已发送未确认
							//数据发送的过程中应该判断是否传输完成
							//为简化过程此处并未实现
							memcpy(&buffer[1],data + 1024 * packetcount_total,1024);
							printf("send a packet with a seq of %d\n",seq_now);
							//cout<<buffer<<endl;
							sendto(sockServer, buffer, BUFFER_LENGTH, 0,(SOCKADDR*)&addrClient, sizeof(SOCKADDR));
							++seq_now;
							if (seq_now > 20) seq_now = seq_now % SEQ_SIZE;
							++packetcount_total;
							Sleep(500);
						}
						recvSize = recvfrom(sockServer,buffer,BUFFER_LENGTH,0,((SOCKADDR*)&addrClient),&length);
						if(recvSize < 0)//没有收到
						{
							counter++;
							if (counter > 20)
							{
								timeoutHandler();
								counter = 0;
							}
						}
						else
						{
							//收到 ack
							ackHandler(buffer[0]);
							counter = 0;
						}
						Sleep(500);
						break;
				}
				if (packetcount_total >(size / 1024)) break;
			}
		}
		if (packetcount_total > size / 1024) break;
		sendto(sockServer, buffer, strlen(buffer)+1, 0, (SOCKADDR*)&addrClient,sizeof(SOCKADDR));
		Sleep(500);
	}
	closesocket(sockServer);
	WSACleanup();
	return 0;
}