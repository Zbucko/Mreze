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

#define SERVER_PORT 20120
#define SERVER_PORT2 20121
#define BUFFER_SIZE 256

struct merenje {
	char ipAddress[32];
	unsigned short port;
	int v;
	int razlika;
};


int main() {
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("WSAStartup failed with error: %d\n", WSAGetLastError());
		return 1;
	}
	int iResult;
	char dataBuffer[BUFFER_SIZE];
	char dataBuffer2[BUFFER_SIZE];

	SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	SOCKET serverSocket2 = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(SERVER_PORT);
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

	sockaddr_in serverAddress2;
	serverAddress2.sin_family = AF_INET;
	serverAddress2.sin_port = htons(SERVER_PORT2);
	serverAddress2.sin_addr.s_addr = htonl(INADDR_ANY);

	iResult = bind(serverSocket, (SOCKADDR*)&serverAddress,
		sizeof(serverAddress));

	iResult = bind(serverSocket2, (SOCKADDR*)&serverAddress2,
		sizeof(serverAddress2));

	sockaddr_in clientAddress;
	int clientAddressSize = sizeof(clientAddress);

	sockaddr_in clientAddress2;
	int clientAddressSize2 = sizeof(clientAddress2);


	iResult = recvfrom(serverSocket, dataBuffer, BUFFER_SIZE, 0,
		(SOCKADDR*)&clientAddress, &clientAddressSize);
	dataBuffer[iResult] = '\0';
	if (strcmp(dataBuffer, "konekcija") == 0) {
		sendto(serverSocket, "uspesna konekcija", strlen("uspesna konekcija")
			, 0, (SOCKADDR*)&clientAddress, sizeof(clientAddress));
	}

	iResult = recvfrom(serverSocket2, dataBuffer2, BUFFER_SIZE, 0,
		(SOCKADDR*)&clientAddress2, &clientAddressSize2);
	dataBuffer2[iResult] = '\0';
	if (strcmp(dataBuffer2, "konekcija") == 0) {
		sendto(serverSocket, "uspesna konekcija", strlen("uspesna konekcija")
			, 0, (SOCKADDR*)&clientAddress2, sizeof(clientAddress2));
	}



	unsigned long mode = 1;
	iResult = ioctlsocket(serverSocket, FIONBIO, &mode);
	if (iResult != NO_ERROR) {
		printf("desila se greska kod ioctlsocket \n");
		return -1;
	}
	iResult = ioctlsocket(serverSocket2, FIONBIO, &mode);
	if (iResult != NO_ERROR) {
		printf("desila se greska kod ioctlsocket \n");
		return -1;
	}
	merenje m[5];
	
	int merenje_count = 0;
	int vlaznost;

	int zadnjeDve[2];

	do {

		iResult = recvfrom(serverSocket, dataBuffer, BUFFER_SIZE, 0,
			(SOCKADDR*)&clientAddress, &clientAddressSize);
		if (iResult != SOCKET_ERROR) {
			dataBuffer[iResult] = '\0';
			vlaznost = atoi(dataBuffer);
			m[merenje_count % 5].v = vlaznost;
			m[merenje_count % 5].port = ntohs(clientAddress.sin_port);
			strcpy_s(m[merenje_count % 5].ipAddress, inet_ntoa(clientAddress.sin_addr));
			printf("poruka sa 1. klijenta (%s : %d) je %d\n", inet_ntoa(clientAddress.sin_addr)
				,ntohs(clientAddress.sin_port) ,vlaznost);
			zadnjeDve[merenje_count % 2] = vlaznost;
			if (merenje_count > 0) {
				m[merenje_count % 5].razlika = abs(zadnjeDve[1] - zadnjeDve[0]);
			}
			else {
				m[merenje_count % 5].razlika = vlaznost;
			}
			merenje_count++;
			if ((merenje_count % 5) == 0) {
				for (int i = 0; i < 5; i++) {
					printf("(%s : %d), %d, razlika je %d\n", m[i].ipAddress,
						m[i].port, m[i].v, m[i].razlika);
				}
			}
		}
		else {
			if (WSAGetLastError() == WSAEWOULDBLOCK) {
				Sleep(1000);
			}
			else {
				printf("desila se neka druga greska\n");
				return -2;
			}
		}

		iResult = recvfrom(serverSocket2, dataBuffer2, BUFFER_SIZE, 0,
			(SOCKADDR*)&clientAddress2, &clientAddressSize2);
		if (iResult != SOCKET_ERROR) {
			dataBuffer2[iResult] = '\0';
			vlaznost = atoi(dataBuffer2);
			m[merenje_count % 5].v = vlaznost;
			m[merenje_count % 5].port = ntohs(clientAddress2.sin_port);
			strcpy_s(m[merenje_count % 5].ipAddress, inet_ntoa(clientAddress2.sin_addr));
			printf("poruka sa 2. klijenta (%s : %d) je %d\n", 
			inet_ntoa(clientAddress2.sin_addr), ntohs(clientAddress2.sin_port), vlaznost);	
			
			zadnjeDve[merenje_count % 2] = vlaznost;
			if (merenje_count > 0) {
				m[merenje_count % 5].razlika = abs(zadnjeDve[1] - zadnjeDve[0]);
			}
			else {
				m[merenje_count % 5].razlika = vlaznost;
			}
			merenje_count++;
			if ((merenje_count % 5) == 0) {
				for (int i = 0; i < 5; i++) {
					printf("(%s : %d), %d, razlika je %d\n", m[i].ipAddress,
						m[i].port, m[i].v, m[i].razlika);
				}
			}
		}
		else {
			if (WSAGetLastError() == WSAEWOULDBLOCK) {
				Sleep(1000);
			}
			else {
				printf("desila se neka druga greska \n");
				return -3;
			}
		}

	} while (true);


	return 0;
}