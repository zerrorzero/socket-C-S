#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <WinSock2.h>  

#define PORT 8087  
#define SERVER_IP "192.168.1.221"  
#define BUFFER_SIZE 1024  
#define FILE_NAME_MAX_SIZE 512  
#pragma comment(lib, "WS2_32")  

int main()
{
	// 声明并初始化一个服务端(本地)的地址结构  
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	//server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	server_addr.sin_addr.S_un.S_addr = inet_addr(SERVER_IP);
	server_addr.sin_port = htons(PORT);

	// 初始化socket dll  
	WSADATA wsaData;
	WORD socketVersion = MAKEWORD(2, 0);
	if (WSAStartup(socketVersion, &wsaData) != 0)
	{
		printf("Init socket dll error!");
		exit(1);
	}

	// 创建socket  
	SOCKET m_Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (SOCKET_ERROR == m_Socket)
	{
		printf("Create Socket Error!");
		exit(1);
	}

	//绑定socket和服务端(本地)地址  
	if (SOCKET_ERROR == bind(m_Socket, (LPSOCKADDR)&server_addr, sizeof(server_addr)))
	{
		printf("Server Bind Failed: %d", WSAGetLastError());
		exit(1);
	}

	//监听  
	if (SOCKET_ERROR == listen(m_Socket, 10))
	{
		printf("Server Listen Failed: %d", WSAGetLastError());
		exit(1);
	}


	while (1)
	{
		printf("Listening To Client...\n");

		sockaddr_in client_addr;
		int client_addr_len = sizeof(client_addr);

		SOCKET m_New_Socket = accept(m_Socket, (sockaddr *)&client_addr, &client_addr_len);
		if (SOCKET_ERROR == m_New_Socket)
		{
			printf("Server Accept Failed: %d", WSAGetLastError());
			break;
		}

		char buffer[BUFFER_SIZE];
		memset(buffer, 0, BUFFER_SIZE);
		if (recv(m_New_Socket, buffer, BUFFER_SIZE, 0) < 0)
		{
			printf("Server Receive Data Failed!");
			break;
		}

		char fn[30], *p;
		char filepath[100] = "c:\\";
		char file[200];
		char file_name[FILE_NAME_MAX_SIZE + 1];
		memset(file_name, 0, FILE_NAME_MAX_SIZE + 1);
		strncpy(file_name, buffer, strlen(buffer)>FILE_NAME_MAX_SIZE ? FILE_NAME_MAX_SIZE : strlen(buffer));
		printf("%s\n", file_name);
		strcpy(fn, (p = strrchr(file_name, '\\')) ? p + 1 : file_name);

		sprintf(file, "%s/%s", filepath, fn);
		printf("%s\n", file);

		FILE * fp = fopen(file , "wb"); //windows下是"wb",表示打开一个只写的二进制文件  
		if (NULL == fp)
		{
			printf("File: %s Can Not Open To Write\n", file);
			system("pause");
			exit(1);
		}
		else
		{
			memset(buffer, 0, BUFFER_SIZE);
			int length = 0;
			while ((length = recv(m_New_Socket, buffer, BUFFER_SIZE, 0)) > 0)
			{
				if (fwrite(buffer, sizeof(char), length, fp) < length)
				{
					printf("File: %s Write Failed\n", file);
					break;
				}
				memset(buffer, 0, BUFFER_SIZE);
			}

			printf("Receive File: %s From Server Successful!\n", file);
			fclose(fp);
		}

		closesocket(m_New_Socket);
	}

	closesocket(m_Socket);
	//释放winsock库  
	WSACleanup();
	return 0;
}
