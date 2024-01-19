// We do not want the warnings about the old deprecated and unsecure CRT functions since these examples can be compiled under *nix as well
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

// Include libraries
#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include "conio.h"
#include "pcap.h"
#include "protocol_headers.h"

char* homophone(char* message, int* key);

int main() {
	int key[] = { 3302, 5, 4, 10, 5812, 21, 99, 83, 7101, 6, 47, 91, 13, 22, 1416, 31, 56, 42, 8, 77, 6652, 51, 39, 46, 24, 29 };
	char message[] = "LEMON";
    printf("Original Message: %s\n", message);

    homophone(message, key);

    printf("Encrypted Message: ");
    for (int i = 0; i < strlen(message); i++) {
        printf("%d ", message[i]);
    }


    return 0;
}

char* homophone(char* message, int* key) {
    for (int i = 0;i < strlen(message);i++) {
        if (message[i] == 'A' || message[i] == 'E' || message[i] == 'I' || message[i] == 'O' || message[i] == 'U') {
            int randomInteger = rand();
            double randomFloat = (double)randomInteger / RAND_MAX;
            if (randomFloat >= 0.5) {
                message[i] = key[message[i] - 'A'] % 100; //higher portion
            }
            else {
                message[i] = key[message[i] - 'A'] / 100; //lower portion
            }
        }
        else {
            message[i] = key[message[i] - 'A'];
        }
     }
    return message;
}