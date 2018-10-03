#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

//��������ͷ�ļ��Ϳ�
#ifdef _WIN32
#include <WinSock2.h>
#include <Windows.h>
#pragma comment (lib, "ws2_32.lib")
#endif

int main(int argc, char** argv)
{
	int ret;

	//��ʼ�����绷��
#ifdef _WIN32
	WSADATA oWsaData;
	WSAStartup(MAKEWORD(2, 2), &oWsaData);
#endif

	//�����ͻ���Socket
	SOCKET oClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (oClient == INVALID_SOCKET) {
		goto FAILED;
	}

	//���ӷ�����
	struct sockaddr_in oServerAddr;
	oServerAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	oServerAddr.sin_family = AF_INET;
	oServerAddr.sin_port = htons(12345);
	int nServerAddrLen = sizeof(oServerAddr);

	ret = connect(oClient, (sockaddr*)&oServerAddr, nServerAddrLen);
	if (ret != 0) {
		goto FAILED;
	}

	//�շ�����
	int nSendLen = send(oClient, "Hello", 5, 0);
	printf("nSendLen = %d\n", nSendLen);

	char pBuffer[128] = {};
	int nRecvLen = recv(oClient, pBuffer, 128, 0);
	printf("nRecvLen = %d; pBuffer = %s\n", nRecvLen, pBuffer);

FAILED:
	//�ر�Socket
	if (oClient != INVALID_SOCKET) {
		closesocket(oClient);
		oClient = INVALID_SOCKET;
	}

	//�������绷��
#ifdef _WIN32
	WSACleanup();
#endif

	system("pause");
	return 0;
}