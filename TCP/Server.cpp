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

#define SERVER_PORT 20022
#define BUFFER_SIZE 256

struct merenje {
	char ipAddress[16];
	unsigned short port;
	int vl;
};

// TCP server that use blocking sockets
int main() {
	WSADATA wsaData; 
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("WSAStartup failed with error: %d\n", WSAGetLastError());
		return 1;
	}
	char dataBuffer[BUFFER_SIZE];
	char dataBuffer2[BUFFER_SIZE];
	int iResult;

	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenSocket == INVALID_SOCKET) {
		printf("neuspelo stvaranje listen socketa u serveru \n");
		return -1;
	}

	sockaddr_in serverAddress;
	memset((char*)&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET; //set server address protocol family 
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(SERVER_PORT);

	iResult = bind(listenSocket, (struct sockaddr*)&serverAddress,
		sizeof(serverAddress));
	if (iResult == SOCKET_ERROR) {
		printf("neuspelo bindovanje socketa u serveru \n");
		return -2;
	}

	iResult = listen(listenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen u serveru nije uspeo\n");
		return -3;
	}

	SOCKET acceptedSocket = INVALID_SOCKET;
	SOCKET acceptedSocket2 = INVALID_SOCKET;


	int vlaznost;
	int rbPorukePrvogServera = 0;
	int prviServerCount = 0;
	merenje m[6];
	
	do {
		sockaddr_in clientAddress;
		int clientAddressSize = sizeof(struct sockaddr_in);

		sockaddr_in clientAddress2;
		int clientAddressSize2 = sizeof(struct sockaddr_in);

		acceptedSocket = accept(listenSocket, (struct sockaddr*)&clientAddress,
			&clientAddressSize);
		if (acceptedSocket == INVALID_SOCKET) {
			printf("nije uspeo accept socket u loop-u servera\n");
			return -8;
		}


		acceptedSocket2 = accept(listenSocket, (struct sockaddr*)&clientAddress2,
			&clientAddressSize2);
		if (acceptedSocket2 == INVALID_SOCKET) {
			printf("nije uspeo accept socket u loop-u servera\n");
			return -8;
		}

		unsigned long mode = 1;
		iResult = ioctlsocket(acceptedSocket, FIONBIO, &mode);
		if (iResult == SOCKET_ERROR) {

		}

		iResult = ioctlsocket(acceptedSocket2, FIONBIO, &mode);
		if (iResult == SOCKET_ERROR) {

		}
		printf("server acceptovao obe konekcije, spremni smo \n");
		
		do {

			iResult = recv(acceptedSocket, dataBuffer, BUFFER_SIZE, 0);
			if (iResult > 0) {
				dataBuffer[iResult] = '\0';
				vlaznost = atoi(dataBuffer);
				rbPorukePrvogServera++;
				m[prviServerCount % 6].vl = vlaznost;
				strcpy_s(m[prviServerCount % 6].ipAddress, inet_ntoa(clientAddress.sin_addr));
				m[prviServerCount % 6].port = ntohs(clientAddress.sin_port);
				printf("trenutna vlaznost je %d, poruka br %d\n", vlaznost,
					rbPorukePrvogServera);

				if (rbPorukePrvogServera % 6 == 0 && rbPorukePrvogServera != 0) {
					for (int i = 0; i < 6; i++) {
						printf("vlaznost je %d, port je %d, IP adresa je %s\n",
							m[i].vl, m[i].port, m[i].ipAddress);
					}
				}
				prviServerCount++;
			}else {
				if (WSAGetLastError() == WSAEWOULDBLOCK) {
					//printf("ceka\n");
					Sleep(1000);
				}
				else {
					printf("doslo je do neke druge greske \n");
					return -10;
				}
			}

			iResult = recv(acceptedSocket2, dataBuffer2, BUFFER_SIZE, 0);
			if (iResult > 0) {
				dataBuffer2[iResult] = '\0';
				vlaznost = atoi(dataBuffer2);
				rbPorukePrvogServera++;
				m[prviServerCount % 6].vl = vlaznost;
				strcpy_s(m[prviServerCount % 6].ipAddress, inet_ntoa(clientAddress2.sin_addr));
				m[prviServerCount % 6].port = ntohs(clientAddress2.sin_port);
				printf("trenutna vlaznost je %d, poruka br %d\n", vlaznost,
					rbPorukePrvogServera);

				if (rbPorukePrvogServera % 6 == 0 && rbPorukePrvogServera != 0) {
					for (int i = 0; i < 6; i++) {
						printf("vlaznost je %d, port je %d, IP adresa je %s\n",
							m[i].vl, m[i].port, m[i].ipAddress);
					}
				}
				prviServerCount++;
			}
			else {
				if (WSAGetLastError() == WSAEWOULDBLOCK) {
					//printf("ceka\n");
					Sleep(1000);
				}
				else {
					printf("doslo je do neke druge greske\n");
				}
			}
		} while (true);
	} while (true);

	shutdown(acceptedSocket, SD_BOTH);
	closesocket(listenSocket);
	closesocket(acceptedSocket);
	WSACleanup();
	return 0;
}