// UDP server that use non-blocking sockets
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

// Port numbers of server that will be used for communication with clients
#define SERVER_PORT1 15011	
#define SERVER_PORT2 15012

// Size of buffer that will be used for sending and receiving messages to clients
#define BUFFER_SIZE 512		

int main()
{
	// Server addresses
	sockaddr_in serverAddress1;
	sockaddr_in serverAddress2;

	// Buffer we will use to send and receive clients' messages
	char dataBuffer[BUFFER_SIZE];

	// WSADATA data structure that is to receive details of the Windows Sockets implementation
	WSADATA wsaData;

	// Initialize windows sockets library for this process
	if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
	{
		printf("WSAStartup failed with error: %d\n", WSAGetLastError());
		return 1;
	}

	// Initialize serverAddress1 and serverAddress2 structures used by bind function
	memset((char*)&serverAddress1, 0, sizeof(serverAddress1));
	serverAddress1.sin_family = AF_INET; 			// set server address protocol family
	serverAddress1.sin_addr.s_addr = INADDR_ANY;	// use all available addresses of server
	serverAddress1.sin_port = htons(SERVER_PORT1);	// 15001 port

	memset((char*)&serverAddress2, 0, sizeof(serverAddress2));
	serverAddress2.sin_family = AF_INET; 							// set server address protocol family
	serverAddress2.sin_addr.s_addr = inet_addr("127.0.0.1");		// loop address
	serverAddress2.sin_port = htons(SERVER_PORT2);					// 15002 port

	// Create first socket
	SOCKET serverSocket1 = socket(AF_INET,      // IPv4 address famly
		SOCK_DGRAM,   // datagram socket
		IPPROTO_UDP); // UDP

	// Check if socket creation succeeded
	if (serverSocket1 == INVALID_SOCKET)
	{
		printf("Creating socket failed with error: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	// Create second socket
	SOCKET serverSocket2 = socket(AF_INET,      // IPv4 address famly
		SOCK_DGRAM,   // datagram socket
		IPPROTO_UDP); // UDP

	// Check if socket creation succeeded
	if (serverSocket2 == INVALID_SOCKET)
	{
		printf("Creating socket failed with error: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	// Bind server address structure (type, port number and local address) to first socket
	int iResult = bind(serverSocket1,(SOCKADDR *)&serverAddress1, sizeof(serverAddress1));

	// Check if socket is succesfully binded to server address data
	if (iResult == SOCKET_ERROR)
	{
		printf("Socket bind failed with error: %d\n", WSAGetLastError());
		closesocket(serverSocket1);
		WSACleanup();
		return 1;
	}

	// Bind server address structure (type, port number and local address) to second socket
	iResult = bind(serverSocket2,(SOCKADDR *)&serverAddress2, sizeof(serverAddress2));

	// Check if socket is succesfully binded to server address data
	if (iResult == SOCKET_ERROR)
	{
		printf("Socket bind failed with error: %d\n", WSAGetLastError());
		closesocket(serverSocket2);
		WSACleanup();
		return 1;
	}

	// Setting non-blocking mode to both sockets
	unsigned long mode = 1;
	if(ioctlsocket(serverSocket1, FIONBIO, &mode) != 0 || ioctlsocket(serverSocket2, FIONBIO, &mode) != 0)
	{
		printf("ioctlsocket failed with error %d\n", WSAGetLastError());
		closesocket(serverSocket1);
		closesocket(serverSocket2);
		WSACleanup();
		return 1;
	}

	printf("Simple UDP server started and waiting client messages.\n");

	// Main server loop
	while (true)
	{
		// Declare and initialize client address that will be set from recvfrom
		sockaddr_in clientAddress;
		memset(&clientAddress, 0, sizeof(clientAddress));

		// Set whole buffer to zero
		memset(dataBuffer, 0, BUFFER_SIZE);

		// Size of client address
		int sockAddrLen = sizeof(clientAddress);

		// Declare and initialize set of socket descriptor for buffer reading
		fd_set readfds;
		FD_ZERO(&readfds);

		// Add socket to set readfds
		FD_SET(serverSocket1, &readfds);
		FD_SET(serverSocket2, &readfds);

		// Declare and initialize set of socket descriptor for exceptions
		fd_set exceptfds;
		FD_ZERO(&exceptfds);

		// Add socket to set exceptfds
		FD_SET(serverSocket1, &exceptfds);
		FD_SET(serverSocket2, &exceptfds);

		// Wait new messages to arrive in one of the sockets in readfds set and wait for possible exception on sockets in exceptfds set
		int sResult = select(0, &readfds , NULL , &exceptfds , NULL);

		// Check whether the error occurred
		if (sResult == SOCKET_ERROR)
		{
			printf("select failed with error: %d\n", WSAGetLastError());
			break;
		}

		// If result is positive, that is indicator that packet has arrived to one or more socket or there is exception on some socket
		// Result number corresonds to number of sockets that have an event
		else if (sResult > 0)
		{
			unsigned long portNumber = 0;

			// We will check if packet is arrived to one of our sockets (if two or more sockets are used, we have to check each socket)
			if(FD_ISSET(serverSocket1, &readfds))
			{
				// Receive client message
				iResult = recvfrom(serverSocket1,			// Socket with port 15001
					dataBuffer,					// Buffer that will be used for receiving message
					BUFFER_SIZE,					// Maximal size of buffer
					0,							// No flags
					(SOCKADDR *)&clientAddress,	// Client information from received message (ip address and port)
					&sockAddrLen);				// Size of sockadd_in structure
				portNumber = SERVER_PORT1;
				if(iResult != SOCKET_ERROR)
				{
					// Set end of string
					dataBuffer[iResult] = '\0';


					printf("Server received message on %d port. Client sent: %s.\n", portNumber,dataBuffer);
				}
				else
				{
					printf("recvfrom failed with error: %d\n", WSAGetLastError());
					continue;
				}
			}
			if(FD_ISSET(serverSocket2, &readfds))
			{
				// Receive client message
				iResult = recvfrom(serverSocket2,			// Socket with port 15002
					dataBuffer,					// Buffer that will be used for receiving message
					BUFFER_SIZE,					// Maximal size of buffer
					0,							// No flags
					(SOCKADDR *)&clientAddress,	// Client information from received message (ip address and port)
					&sockAddrLen);				// Size of sockadd_in structure
				portNumber = SERVER_PORT2;
				if(iResult != SOCKET_ERROR)
				{
					// Set end of string
					dataBuffer[iResult] = '\0';


					printf("Server received message on %d port. Client sent: %s.\n", portNumber,dataBuffer);
				}
				else
				{
					printf("recvfrom failed with error: %d\n", WSAGetLastError());
					continue;
				}
			}
			// We will check if an exception occurred on one of our sockets. Close server application if error occured
			if(FD_ISSET(serverSocket1, &exceptfds))
			{
				break;
			}
			if(FD_ISSET(serverSocket2, &exceptfds))
			{
				break;
			}
			// neither of two socket is in these two sets (some error has occurred)
			else
			{
				continue;
			}

			// Check if message is succesfully received

		}
	}

	// Close server application
	if (SOCKET_ERROR == closesocket(serverSocket1) || SOCKET_ERROR == closesocket(serverSocket2))
	{
		printf("closesocket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	printf("Server successfully shut down.\n");

	// Close Winsock library
	WSACleanup();
	return 0;
}