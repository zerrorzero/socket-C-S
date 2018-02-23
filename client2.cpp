#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <string>
#include <io.h>  
#include <fstream>  
#include <vector>  
#include <iostream>
#include <WinSock2.h>
#include"findname.h"
using namespace std;

#define PORT 8087  
#define SERVER_IP "192.168.1.221"  
#define BUFFER_SIZE 1024  
#define FILE_NAME_MAX_SIZE 512  
#pragma comment(lib, "WS2_32")  

int main()
{
	// 初始化socket dll  
	WSADATA wsaData;
	WORD socketVersion = MAKEWORD(2, 0);
	if (WSAStartup(socketVersion, &wsaData) != 0)
	{
		printf("Init socket dll error!");
		exit(1);
	}



	//指定服务端的地址  
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = inet_addr(SERVER_IP);
	server_addr.sin_port = htons(PORT);


	
	while (1)
	{
		/*
		//创建socket  
		SOCKET c_Socket = socket(AF_INET, SOCK_STREAM, 0);
		if (SOCKET_ERROR == c_Socket)
		{
			printf("Create Socket Error!");
			system("pause");
			exit(1);
		}

		if (SOCKET_ERROR == connect(c_Socket, (LPSOCKADDR)&server_addr, sizeof(server_addr)))
		{
			printf("Can Not Connect To Server IP!\n");
			system("pause");
			exit(1);
		}
		*/
		//输入文件名  
		char file_name[FILE_NAME_MAX_SIZE + 1];
		memset(file_name, 0, FILE_NAME_MAX_SIZE + 1);
		printf("Please Input File Name On Client: ");
		scanf("%s", &file_name);

		string filePath = file_name;
		vector<string> files;
		

		//读取所有的文件，包括子文件的文件  
		GetAllFiles(filePath, files);

		int size = files.size();


		for (int i = 0; i<size; i++)
		{
			//创建socket  
			SOCKET c_Socket = socket(AF_INET, SOCK_STREAM, 0);
			if (SOCKET_ERROR == c_Socket)
			{
				printf("Create Socket Error!");
				system("pause");
				exit(1);
			}

			if (SOCKET_ERROR == connect(c_Socket, (LPSOCKADDR)&server_addr, sizeof(server_addr)))
			{
				printf("Can Not Connect To Server IP!\n");
				system("pause");
				exit(1);
			}
			char filename[FILE_NAME_MAX_SIZE + 1];
			strcpy(filename, files[i].c_str());

			char buffer[BUFFER_SIZE];
			memset(buffer, 0, BUFFER_SIZE);
			strncpy(buffer, filename, strlen(filename)>BUFFER_SIZE ? BUFFER_SIZE : strlen(filename));

			//向服务器发送文件名  
			if (send(c_Socket, buffer, BUFFER_SIZE, 0) < 0)
			{
				printf("Send File Name Failed\n");
				system("pause");
				exit(1);
			}

			//打开文件，准备写入  
			FILE * fp = fopen(filename, "rb");  //windows下是"rb",表示打开一个只读的二进制文件  
			if (NULL == fp)
			{
				printf("File: %s Not Found\n", filename);
			}
			else
			{
				memset(buffer, 0, BUFFER_SIZE);
				int length = 0;

				while ((length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0)
				{
					if (send(c_Socket, buffer, length, 0) < 0)
					{
						printf("Send File: %s Failed\n", filename);
						break;
					}
					memset(buffer, 0, BUFFER_SIZE);
				}

				fclose(fp);
				printf("File: %s Transfer Successful!\n", filename);

			}
			closesocket(c_Socket);
		}
		
		/*
		char buffer[BUFFER_SIZE];
		memset(buffer, 0, BUFFER_SIZE);
		strncpy(buffer, file_name, strlen(file_name)>BUFFER_SIZE ? BUFFER_SIZE : strlen(file_name));

		//向服务器发送文件名  
		if (send(c_Socket, buffer, BUFFER_SIZE, 0) < 0)
		{
			printf("Send File Name Failed\n");
			system("pause");
			exit(1);
		}

		//打开文件，准备写入  
		FILE * fp = fopen(file_name, "rb");  //windows下是"rb",表示打开一个只读的二进制文件  
		if (NULL == fp)
		{
			printf("File: %s Not Found\n", file_name);
		}
		else
		{
			memset(buffer, 0, BUFFER_SIZE);
			int length = 0;

			while ((length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0)
			{
				if (send(c_Socket, buffer, length, 0) < 0)
				{
					printf("Send File: %s Failed\n", file_name);
					break;
				}
				memset(buffer, 0, BUFFER_SIZE);
			}

			fclose(fp);
			printf("File: %s Transfer Successful!\n", file_name);

		}
		closesocket(c_Socket);*/
	}
	
	
	//释放winsock库  
	WSACleanup();

	system("pause");
	return 0;
}