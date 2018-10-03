#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

//��������ͷ�ļ��Ϳ�
#ifdef WIN32
#include <WinSock2.h>
#include <Windows.h>
#pragma comment (lib, "ws2_32.lib")
#endif

int main(int argc, char** argv)
{
	int ret;

	//��ʼ�����绷��
#ifdef WIN32
	WSADATA oWsaData;
	WSAStartup(MAKEWORD(2, 2), &oWsaData);
#endif

	//����������Socket
	SOCKET oServer = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (oServer == INVALID_SOCKET) {
		goto FAILED;
	}

	//�󶨶˿�
	struct sockaddr_in oServerAddr;
	oServerAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	oServerAddr.sin_family = AF_INET;
	oServerAddr.sin_port = htons(12345);
	int nServerAddrLen = sizeof(oServerAddr);
	ret = bind(oServer, (const struct sockaddr*)&oServerAddr, nServerAddrLen);
	if (ret != 0) {
		goto FAILED;
	}

	while (1) {
		//�շ�����
		struct sockaddr_in oClientAddr;
		int nClientAddrLen = sizeof(oClientAddr);

		char pBuffer[128] = {};
		memset(pBuffer, 0, 128);
		int nRecvLen = recvfrom(oServer, pBuffer, 128, 0, (sockaddr*)&oClientAddr, &nClientAddrLen);
		printf("nRecvLen = %d; pBuffer = %s\n", nRecvLen, pBuffer);

		int nSendLen = sendto(oServer, pBuffer, nRecvLen, 0, (sockaddr*)&oClientAddr, nClientAddrLen);
		printf("nSendLen = %d\n", nSendLen);
	}

FAILED:
	//�ر�Socket
	if (oServer != INVALID_SOCKET) {
		closesocket(oServer);
	}

	//�������绷��
#ifdef WIN32
	WSACleanup();
#endif

	system("pause");
	return 0;
}