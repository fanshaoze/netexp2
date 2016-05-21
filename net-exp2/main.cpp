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
	//���ջ�����
	char buffer[buffer_length]; 
	ZeroMemory(buffer,sizeof(buffer));
	int len = sizeof(SOCKADDR);
	//Ϊ�˲���������������ӣ�����ʹ�� -time ����ӷ������˻�õ�ǰʱ��
	//ʹ�� -testgbn [X] [Y] ���� GBN ����[X]��ʾ���ݰ���ʧ����
	// [Y]��ʾ ACK �������� 
	printTips();
	int ret;
	int interval = 1;//�յ����ݰ�֮�󷵻� ack �ļ����Ĭ��Ϊ 1 ��ʾÿ�������� ack�� 0 ���߸�������ʾ���еĶ������� ack
	float packetLossRate = 0.2; //Ĭ�ϰ���ʧ�� 0.2
	float ackLossRate = 0.2; //Ĭ�� ACK ��ʧ�� 0.2

	//��ʱ����Ϊ������ӣ�����ѭ����������
	srand((unsigned)time(NULL));
	while(true)
	{
		gets_s(buffer);
		ret = sscanf(buffer,"%s%f%f",&command,&packetLossRate,&ackLossRate);
		/*
		����ԭ��:
		int sscanf( const char *, const char *, ...);
		int sscanf(const char *buffer,const char *format,[argument ]...);
		buffer�洢������
		format��ʽ�����ַ���
		argument ѡ�����趨�ַ���
		sscanf���buffer��������ݣ�����format�ĸ�ʽ������д�뵽argument�*/
		//��ʼ GBN ���ԣ�ʹ�� GBN Э��ʵ�� UDP �ɿ��ļ�����
		if(!strcmp(command,"-testgbn"))
		{
			cout<<"Begin to test GBN protocol, please don't abort theprocess"<<endl;
			printf("The loss ratio of packet is %.2f,the loss ratio of ack is %.2f\n",packetLossRate,ackLossRate);
			int waitCount = 0;
			int stage = 0;
			BOOL b;
			unsigned char state_code;//״̬��
			unsigned short packet_number;//�������к�
			unsigned short recv_number;//���մ��ڴ�СΪ 1����ȷ�ϵ����к�
			unsigned short expect_number;//�ȴ������к�
			sendto(socketClient, "-testgbn", strlen("-testgbn")+1, 0, (SOCKADDR*)&addrServer, sizeof(SOCKADDR));
			while (true)
			{
				//�ȴ� server �ظ����� UDP Ϊ����ģʽ
				recvfrom(socketClient,buffer,buffer_length,0,(SOCKADDR*)&addrServer, &len);
				//cout<<buffer<<endl;
				switch(stage)
				{
					case 0://�ȴ����ֽ׶�
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
					case 1://�ȴ��������ݽ׶�
						packet_number = (unsigned short)buffer[0];
						b = JudgeLoss(packetLossRate);
						if(b)
						{
							cout<<"The packet with a packet_number of "<<(unsigned short)packet_number<<" loss"<<endl;
							continue;
						}
						cout<<"recv a packet with a packet_number of "<<(unsigned short)packet_number<<endl;
						//������ڴ��İ�����ȷ���գ�����ȷ�ϼ���
						if(expect_number == packet_number)
						{
							++expect_number;
							if(expect_number == 21)	expect_number = 1;
							//�������
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
								buffer[0] = recv_number;//��һ�ε��Ǹ�ack��ֵ
								buffer[1] = '\0';
							}
						}
						//ackͬ��Ҳ�Ǹ��ʵĶ���!!!!!!!!!
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
		//GBN����,
		sendto(socketClient, buffer, strlen(buffer)+1, 0,(SOCKADDR*)&addrServer, sizeof(SOCKADDR));
		ret = recvfrom(socketClient,buffer,buffer_length,0,(SOCKADDR*)&addrServer,&len);
		cout<<buffer<<endl;
		if(!strcmp(buffer,"Good bye!")) break;
		else printTips();
	}
	//�ر��׽���
	closesocket(socketClient);
	WSACleanup();
	return 0;
}

