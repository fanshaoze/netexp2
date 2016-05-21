// server.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

//������
int main(int argc, char* argv[])
{
	int size;
	int err;
	//�����׽��ֿ⣨���룩 
	err = InitSock();
	SOCKET sockServer = socket(AF_INET, SOCK_DGRAM,IPPROTO_UDP);
	//��ַ��ʽ��udp��udpЭ��
	//�����׽���Ϊ������ģʽ
	int iMode = 1; //1���������� 0������
	ioctlsocket(sockServer,FIONBIO, (u_long FAR*) &iMode);//����������
	//#define FIONBIO     _IOW('f', 126, u_long) /* set/clear non-blocking i/o */
	/*int ioctlsocket( int s, long cmd, u_long * argp);
	s��һ����ʶ�׽ӿڵ������֡�
	cmd�����׽ӿ�s�Ĳ������
	argp��ָ��cmd��������������ָ�롣
	ע�ͣ��༭��������������һ״̬����һ�׽ӿڡ������ڻ�ȡ���׽ӿ���صĲ����������������Э���ͨѶ��ϵͳ�޹ء�֧���������
	*/
	SOCKADDR_IN addrServer; //��������ַ
	//addrServer.sin_addr.S_un.S_addr = inet_addr(SERVER_IP);
	addrServer.sin_addr.S_un.S_addr = htonl(INADDR_ANY);//���߾���
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(SERVER_PORT);
	err = bind(sockServer,(SOCKADDR*)&addrServer, sizeof(SOCKADDR));
	//bind���Ϊ��socket�ṹ�����Ip��ַ�Ͷ˿ںŸ�ֵ
	if(err)
	{
		err = GetLastError();//���bind������û����ȷִ��
		printf("Could not bind the port %d for socket.Error codeis %d\n",SERVER_PORT,err);

		WSACleanup();
		return -1;
	}
	SOCKADDR_IN addrClient; //�ͻ��˵�ַ
	int length = sizeof(SOCKADDR);
	char buffer[BUFFER_LENGTH]; //���ݷ��ͽ��ջ�����
	ZeroMemory(buffer,sizeof(buffer));
	FILE* file = fopen("ReadMe.txt", "rb");
	if (file)
	{
		size = _filelength(_fileno(file));
		cout << size << endl;
		fclose(file);
	}
	//���������ݶ����ڴ�
	std::ifstream icin;
	//ifstream file2("c:\\pdos.def");�����뷽ʽ���ļ�
	icin.open("test.txt");//?????�����ļ���������
	char *data = new char[size];
	ZeroMemory(data,sizeof(data));
	icin.read(data,size);
	//��read()���ļ��ж�ȡ num ���ַ��� buf ָ��Ļ����У�
	//http://www.cppblog.com/saga/archive/2007/06/19/26652.html fstream�ľ��庯���Ĺ��ܺ��÷�
	icin.close();
	packetcount_total = size / 1024;
	int recvSize ;
	for(int i=1; i <= SEQ_SIZE; ++i)	ack[i] = TRUE;
	while(true)
	{
		//���������գ���û���յ����ݣ�����ֵΪ-1
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
			//���� gbn ���Խ׶�
			//���� server�� server ���� 0 ״̬���� client ���� 205 ״̬�루 server���� 1 ״̬��

			//server �ȴ� client �ظ� 200 ״̬�룬����յ��� server ���� 2 ״̬������ʼ�����ļ���������ʱ�ȴ�ֱ����ʱ\
			//���ļ�����׶Σ� server ���ʹ��ڴ�С��Ϊ
			ZeroMemory(buffer,sizeof(buffer));
			int recvSize;
			int counter = 0;
			cout<<"Begain to test GBN protocol"<<endl;
			//������һ�����ֽ׶�
			//���ȷ�������ͻ��˷���һ�� 205 ��С��״̬�루���Լ�����ģ���ʾ������׼�����ˣ����Է�������
			//�ͻ����յ� 205 ֮��ظ�һ�� 200 ��С��״̬�룬��ʾ�ͻ���׼�����ˣ����Խ���������
			//�������յ� 200 ״̬��֮�󣬾Ϳ�ʼʹ�� GBN ����������
			cout<<"Shake hands stage"<<endl;
			int stage = 0;
			bool runFlag = true;
			while(runFlag)
			{
				switch(stage)
				{
					case 0://���� 205 �׶�
						buffer[0] = 205;
						sendto(sockServer, buffer, strlen(buffer)+1, 0,(SOCKADDR*)&addrClient, sizeof(SOCKADDR));
						/*
						s �׽���
						buff ���������ݵĻ�����
						size ����������
						Flags ���÷�ʽ��־λ, һ��Ϊ0, �ı�Flags������ı�Sendto���͵���ʽ
						addr ����ѡ��ָ�룬ָ��Ŀ���׽��ֵĵ�ַ
						len addr��ָ��ַ�ĳ���
						*/
						Sleep(100);
						stage = 1;
						break;
					case 1://�ȴ����� 200 �׶Σ�û���յ��������+1����ʱ������˴Ρ����ӡ����ȴ��ӵ�һ����ʼ
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
					case 2://���ݴ���׶Σ����͸��ͻ��˵����кŴ� 1 ��ʼ
						if(seqIsAvailable())
						{
						
							buffer[0] = seq_now;
							ack[seq_now] = FALSE;
							//�ڿ�ʼ��ʱ��Ҫ�ѵ�ǰ��������ݵ�λ����Ϊfalse��Ҳ�����ѷ���δȷ��
							//���ݷ��͵Ĺ�����Ӧ���ж��Ƿ������
							//Ϊ�򻯹��̴˴���δʵ��
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
						if(recvSize < 0)//û���յ�
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
							//�յ� ack
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