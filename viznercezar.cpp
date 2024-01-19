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

void packet_handler(unsigned char* fd, const struct pcap_pkthdr*
	packet_header, const unsigned char* packet_data);
pcap_dumper_t* file_dumper;
int arp_counter = 0;
char key[] = "QVIDE";//Ceaser cypher of word "MREZA" using step 4
char* encrypt_data(char* message, char* key);


int main()
{
	pcap_t* device_handle;
	char error_buffer[PCAP_ERRBUF_SIZE];

	// Open the capture file
	if ((device_handle = pcap_open_offline("packetsv12.pcap", // Name of the device
		error_buffer // Error buffer
	)) == NULL)
	{
		printf("\n Unable to open the file %s.\n", "packetsv12.pcap");
		return -1;
	}

	// Check the link layer. We support only Ethernet for simplicity.
	if (pcap_datalink(device_handle) != DLT_EN10MB)
	{
		printf("\nThis program works only on Ethernet networks.\n");
		return -1;
	}

	file_dumper = pcap_dump_open(device_handle, "encrypackets.pcap");
	if (file_dumper == NULL)
	{
		printf("\n Error opening output file\n");
		return -1;
	}

	char filter_exp[] = "ip and tcp";
	struct bpf_program fcode;

	// Compile the filter
	if (pcap_compile(device_handle, &fcode, filter_exp, 1, 0xffffff) < 0)
	{
		printf("\n Unable to compile the packet filter. Check the syntax.\n");
		return -1;
	}
	// Set the filter
	if (pcap_setfilter(device_handle, &fcode) < 0)
	{
		printf("\n Error setting the filter.\n");
		return -1;
	}


	printf("ARP counter:%d", arp_counter);


	// Read and dispatch packets until EOF is reached
	pcap_loop(device_handle, 10, packet_handler, NULL);
	// Close the file associated with device_handle and deallocates resources
	pcap_close(device_handle);

	getchar();
	return 0;
}

void packet_handler(unsigned char* fd, const struct pcap_pkthdr*
	packet_header, const unsigned char* packet_data)
{

	int velicinaPaketa = packet_header->len;
	printf("Packet length: %ld bytes\n", velicinaPaketa);
	char kopija[10000];
	memset(kopija, 0, velicinaPaketa * sizeof(char));
	ethernet_header* eh = (ethernet_header*)packet_data;
	memcpy(kopija, eh, sizeof(ethernet_header));

	printf("Dest MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
		eh->dest_address[0], eh->dest_address[1], eh->dest_address[2],
		eh->dest_address[3], eh->dest_address[4], eh->dest_address[5]);

	if(ntohs(eh->type == 0x0806)){
		printf("ARP Protokol:");
		arp_counter++;
	}
	else if (ntohs(eh->type) == 0x800) {
		ip_header* ih = (ip_header*)(packet_data + sizeof(ethernet_header));
		memcpy(kopija + sizeof(ethernet_header), ih, ih->header_length * 4);

		printf("%u", ih->ttl); // Ako ne valja ispis promeniti %u

		if (ih->next_protocol == 0x6) {
			tcp_header* th = (tcp_header*)((unsigned char*)ih + ih->header_length*4);
			printf("Dest port: %u\n", ntohs(th->dest_port));
			printf("Window size: %u\n", ntohs(th->windows_size));

			if (th->flags == 16 && th->sequence_num == 0) {
				printf("ACK flag detected and seq num=0;\n");
				printf("Source port: %u\n", ntohs(th->src_port));
			}

			if (ntohs(th->src_port) == 80 || ntohs(th->dest_port) == 80) {
				printf("HTTP protocol data: \n");
				char* app_data = (char*)((unsigned char*)th + th->header_length * 4);
				for (int i = 0;i < 16;i++) {
					printf("%c", app_data[i]);
				}
				printf("\n");
			}

		}
		else if (ih->next_protocol == 0x11) {
			udp_header* uh = (udp_header*)((unsigned char*)ih + ih->header_length *4);
			memcpy(kopija + sizeof(ethernet_header) + ih->header_length * 4, uh, sizeof(uh));
			char* app_data = (char*)((unsigned char*)uh + sizeof(udp_header));
			printf("UDP: Packet size: %u\n", ntohs(uh->datagram_length));

			char* coded = encrypt_data(app_data, key);
			memcpy(kopija + sizeof(ethernet_header) + ih->header_length * 4 + sizeof(udp_header), coded, strlen(coded));
			pcap_dump((unsigned char*)fd, packet_header, (const unsigned char*)kopija);
		}
	}

}

char* encrypt_data(char* message, char* key) {
	size_t messageLen = strlen(message);
	size_t keyLen = strlen(key);

	if (messageLen == 0 || keyLen == 0) {
		return NULL;
	}

	for (size_t i = 0;i < messageLen;i++) {
		if (message[i] >= 'A' && message[i] <= 'Z') {
			message[i] = 'A' + (message[i] - 'A' + key[i % keyLen] - 'A') % 26;
		}
		else if (message[i] >= 'a' && message[i] <= 'z') {
			message[i] = 'A' + (message[i] - 'A' + key[i % keyLen] - 'A') % 26;
		}
	}

	return message;
}
