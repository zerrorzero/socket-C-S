#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <string>
#include <io.h> 
#include <fstream>  
#include <vector>  
#include <iostream>
#include <WinSock2.h>
using namespace std;

#define BUFFER_SIZE 4096  
#define CMD_MAX_SIZE 512  
#pragma comment(lib, "WS2_32")  

int main(int argc, TCHAR *argv[])
{
	TCHAR    *SERVER_IP;
	u_short    PORT;
	TCHAR    CmdStr[CMD_MAX_SIZE];
	TCHAR    buffer[BUFFER_SIZE];

	if (argc != 3)
	{
		printf("usage: \"%s\" <server IP> <PORT> \n", argv[0]);
		return 1;
	}
	else
	{
		SERVER_IP = argv[1];
		PORT = atoi(argv[2]);

		// initialize socket dll  
		WSADATA wsaData;
		WORD socketVersion = MAKEWORD(2, 0);
		if (WSAStartup(socketVersion, &wsaData) != 0)
		{
			printf("Init socket dll error!");
			exit(1);
		}

		sockaddr_in server_addr;
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.S_un.S_addr = inet_addr(SERVER_IP);
		server_addr.sin_port = htons(PORT);

		sockaddr_in client_addr;
		client_addr.sin_family = AF_INET;
		client_addr.sin_addr.S_un.S_addr = htons(INADDR_ANY);
		client_addr.sin_port = htons(0);


		while (1)
		{
			SOCKET c_Socket = socket(AF_INET, SOCK_STREAM, 0);
			if (SOCKET_ERROR == c_Socket)
			{
				printf("Create Socket Error: %d", WSAGetLastError());
				system("pause");
				exit(1);
			}

			if (SOCKET_ERROR == bind(c_Socket, (LPSOCKADDR)&client_addr, sizeof(client_addr)))
			{
				printf("Client Bind Failed: %d", WSAGetLastError());
				exit(1);
			}

			if (SOCKET_ERROR == connect(c_Socket, (LPSOCKADDR)&server_addr, sizeof(server_addr)))
			{
				printf("Can Not Connect To Server IP!\n");
				system("pause");
				exit(1);
			}

			memset(CmdStr, 0, CMD_MAX_SIZE + 1);
			printf("Please Input command On Client: ");
			fgets(CmdStr, CMD_MAX_SIZE+1, stdin);

			TCHAR *find = strchr(CmdStr, '\n');
			if (find)
			{
				*find = '\0';
			}

			//send the command to server
			if (send(c_Socket, CmdStr, CMD_MAX_SIZE, 0) < 0)
			{
				printf("Send command Failed\n");
				system("pause");
				exit(1);
			}

			//recv the result form server
			memset(buffer, 0, BUFFER_SIZE);
			if (recv(c_Socket, buffer, BUFFER_SIZE, 0) < 0)
			{
				printf("Send File Name Failed\n");
				system("pause");
				exit(1);
			}
			printf("\n[result]:\n%s\n", buffer);

			closesocket(c_Socket);

		}
 
		WSACleanup();

		system("pause");
		return 0;
	}
}
