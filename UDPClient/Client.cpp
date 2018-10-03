#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

//网络连接头文件和库
#ifdef _WIN32
#include <WinSock2.h>
#include <Windows.h>
#pragma comment (lib, "ws2_32.lib")
#endif

int main(int argc, char** argv)
{
	int ret;

	//初始化网络环境
#ifdef _WIN32
	WSADATA oWsaData;
	WSAStartup(MAKEWORD(2, 2), &oWsaData);
#endif

	//创建客户端Socket
	SOCKET oClient = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (oClient == INVALID_SOCKET) {
		goto FAILED;
	}

	//发数据
	struct sockaddr_in oToAddr;
	oToAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	oToAddr.sin_family = AF_INET;
	oToAddr.sin_port = htons(12345);
	int nToAddrLen = sizeof(oToAddr);

	int nSendLen = sendto(oClient, "Hello", strlen("Hello"), 0, (sockaddr*)&oToAddr, nToAddrLen);
	printf("nSendLen = %d\n", nSendLen);

	//收数据
	char pBuffer[128] = {};
	struct  sockaddr_in oFromAddr;
	int nFromAddrLen = sizeof(oFromAddr);

	int nRecvLen = recvfrom(oClient, pBuffer, 128, 0, (sockaddr*)&oFromAddr, &nFromAddrLen);
	printf("nRecvLen = %d; pBuffer = %s\n", nRecvLen, pBuffer);

FAILED:
	//关闭Socket
	if (oClient != INVALID_SOCKET) {
		closesocket(oClient);
	}

	//销毁网络环境
#ifdef _WIN32
	WSACleanup();
#endif

	system("pause");
	return 0;
}