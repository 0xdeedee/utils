#include "stdafx.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS


#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdio.h>

// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")


// add pairs of file name and fp to avoid permanent opnening of the files 
static void write_to_file(char *b, int b_sz, char *addr)
{
	char	file_name[FILENAME_MAX];

	sprintf(file_name, "neuroscan_debug_%s.log", addr);
	FILE	*fp = fopen(file_name, "a+");
	fputs(b, fp);
	fclose(fp);
}

int _tmain(int argc, _TCHAR* argv[])
{
	int						res = 0;
	WSADATA					wsa_data;
	SOCKET					sock;
	sockaddr_in				recv_addr;
	unsigned short			port = 4567;
	char					buff[1024];
	int						buff_sz = 1024;
	sockaddr_in				sender_addr;
	int						sender_addr_sz = sizeof(sockaddr_in);


	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsa_data))
	{
		wprintf(L"WSAStartup failed with error %d\n", res);
		return 1;
	}

	if (INVALID_SOCKET == (sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)))
	{
		wprintf(L"socket failed with error %d\n", WSAGetLastError());
		return 1;
	}

	recv_addr.sin_family = AF_INET;
	recv_addr.sin_port = htons(port);
	recv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (0 !=  bind(sock, (SOCKADDR *)& recv_addr, sizeof(recv_addr)))
	{
		wprintf(L"bind failed with error %d\n", WSAGetLastError());
		return 1;
	}

	wprintf(L"Receiving datagrams...\n");
	while (1)
	{
		memset(buff, 0, buff_sz);
		if (SOCKET_ERROR == (res = recvfrom(sock, buff, buff_sz, 0, (SOCKADDR *)& sender_addr, &sender_addr_sz) ) )
		{
			wprintf(L"recvfrom failed with error %d\n", WSAGetLastError());
		}
		else
		{
			wprintf(L"recvfrom failed with no error\n");
			write_to_file(buff, buff_sz, inet_ntoa(sender_addr.sin_addr));
		}
	}
	wprintf(L"Finished receiving. Closing socket.\n");

	if ( SOCKET_ERROR == closesocket( sock ) )
	{
		wprintf(L"closesocket failed with error %d\n", WSAGetLastError());
		return 1;
	}

	wprintf(L"Exiting.\n");
	WSACleanup();
	return 0;
}

