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
	char dataBuffer[BUFFER_SIZE];
	int iResult;

	SOCKET connectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (connectSocket == INVALID_SOCKET) {
		printf("neuspesno pravljenje connect socketa u clientu\n");
		return -5;
	}
	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP_ADDRESS);
	serverAddress.sin_port = htons(SERVER_PORT);

	iResult = connect(connectSocket, (SOCKADDR*)&serverAddress,
		sizeof(serverAddress));
	if (iResult == SOCKET_ERROR) {
		printf("nije uspeo connect u clientu \n");
		return -6;
	}

	printf("konektovani smo na server\n");

	do{
		printf("unesi poruku koja treba da propluta do servera ");
		gets_s(dataBuffer, BUFFER_SIZE);

		iResult = send(connectSocket, dataBuffer, (int)strlen(dataBuffer), 0);
		if (iResult == SOCKET_ERROR) {
			printf("nije uspeo send iz klijenta \n");
			return -7;
		}
		if (!strcmp(dataBuffer, "kraj")) {
			break;
		}

	} while (true);

	shutdown(connectSocket, SD_BOTH);
	closesocket(connectSocket);
	WSACleanup();
	return 0;
}