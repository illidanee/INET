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
	SOCKET oClient = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (oClient == INVALID_SOCKET) {
		goto FAILED;
	}

	//������
	struct sockaddr_in oToAddr;
	oToAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	oToAddr.sin_family = AF_INET;
	oToAddr.sin_port = htons(12345);
	int nToAddrLen = sizeof(oToAddr);

	int nSendLen = sendto(oClient, "Hello", strlen("Hello"), 0, (sockaddr*)&oToAddr, nToAddrLen);
	printf("nSendLen = %d\n", nSendLen);

	//������
	char pBuffer[128] = {};
	struct  sockaddr_in oFromAddr;
	int nFromAddrLen = sizeof(oFromAddr);

	int nRecvLen = recvfrom(oClient, pBuffer, 128, 0, (sockaddr*)&oFromAddr, &nFromAddrLen);
	printf("nRecvLen = %d; pBuffer = %s\n", nRecvLen, pBuffer);

FAILED:
	//�ر�Socket
	if (oClient != INVALID_SOCKET) {
		closesocket(oClient);
	}

	//�������绷��
#ifdef _WIN32
	WSACleanup();
#endif

	system("pause");
	return 0;
}