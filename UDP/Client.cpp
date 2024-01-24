#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "conio.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define SERVER_IP_ADDRESS "127.0.0.1"
#define SERVER_PORT 20022
#define BUFFER_SIZE 256

// TCP client that use blocking sockets
int main() {
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("WSAStartup failed with error: %d\n", WSAGetLastError());
		return 1;
	}
	int iResult;
	char dataBuffer[BUFFER_SIZE];

	SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	unsigned short port;
	printf("unesite port (20120 || 20121) ");
	gets_s(dataBuffer, BUFFER_SIZE);
	port = atoi(dataBuffer);
	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);
	serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP_ADDRESS);

	int serverAddressSize = sizeof(serverAddress);
	
	
	
	iResult = sendto(clientSocket, "konekcija", strlen("konekcija"), 0
		, (SOCKADDR*)&serverAddress, sizeof(serverAddress));

	iResult = recvfrom(clientSocket, dataBuffer, BUFFER_SIZE, 0,
		NULL, NULL);
	if (iResult > 0) {
		dataBuffer[iResult] = '\0';
		printf("message received from server %s\n", dataBuffer);
	}else {
		printf("recv failed\n");
		closesocket(clientSocket);
		WSACleanup();
	}
	

	do {
		printf("unesi vlaznost za ovo merenje (unesi ceo broj nemoj se glupirati)  ");
		gets_s(dataBuffer, BUFFER_SIZE);

		iResult = sendto(clientSocket, dataBuffer, strlen(dataBuffer), 0
			, (SOCKADDR*)&serverAddress, sizeof(serverAddress));
	} while (true);

	return 0;
}