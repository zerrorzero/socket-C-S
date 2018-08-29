#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <string>
#include <io.h>  
#include <fstream>  
#include <vector>  
#include <iostream>
#include <WinSock2.h>
#include <atlbase.h>
#include <wchar.h>
#include <time.h>
using namespace std;

#define BUFFER_SIZE 1024  
#define FILE_NAME_MAX_SIZE 512  
#pragma comment(lib, "WS2_32")  


#define F(x,y,z) ((x & y) | (~x & z))
#define G(x,y,z) ((x & z) | (y & ~z))
#define H(x,y,z) (x^y^z)
#define I(x,y,z) (y ^ (x | ~z))
#define ROTATE_LEFT(x,n) ((x << n) | (x >> (32-n)))

//FF =the result of a nonlinear function + a content's subgroup + a constant + the first variable,
//and,rotate left s bit.Then add the second variable.Finaly assign to the first variable.
#define FF(a,b,c,d,x,s,ac){a += F(b, c, d) + x + ac; a = ROTATE_LEFT(a, s); a += b; }
#define GG(a,b,c,d,x,s,ac){a += G(b, c, d) + x + ac; a = ROTATE_LEFT(a, s); a += b; }
#define HH(a,b,c,d,x,s,ac){a += H(b, c, d) + x + ac; a = ROTATE_LEFT(a, s); a += b; }
#define II(a,b,c,d,x,s,ac){a += I(b, c, d) + x + ac; a = ROTATE_LEFT(a, s); a += b; }

typedef struct {
	unsigned int count[2];
	unsigned int state[4];
	unsigned char buffer[64];
} MD5_CTX;

void MD5Init(MD5_CTX *context);
void MD5Update(MD5_CTX *context, unsigned char *input, unsigned int inputlen);
void MD5Final(MD5_CTX *context, unsigned char digest[16]);
void MD5Transform(unsigned int state[4], unsigned char block[64]);
void MD5Encode(unsigned char *output, unsigned int *input, unsigned int len);
void MD5Decode(unsigned int *output, unsigned char *input, unsigned int len);

//If subgroup enough 512bit,add padding to 512bit.
unsigned char PADDING[] = {
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

//Initialize the MD5_CTX,and state[4] is chaining variable.
void MD5Init(MD5_CTX *context) {
	context->count[0] = 0;
	context->count[1] = 0;
	context->state[0] = 0x67452301;
	context->state[1] = 0xEFCDAB89;
	context->state[2] = 0x98BADCFE;
	context->state[3] = 0x10325476;
}

void MD5Update(MD5_CTX *context, unsigned char *input, unsigned int inputlen)  
{
	unsigned int i = 0, index = 0, partlen = 0;
	index = (context->count[0] >> 3) & 0x3F; //The same as (context->count[0]/8)%64
	partlen = 64 - index;
	context->count[0] += inputlen << 3;
	//If 32bit not enough,add count[1] as 64bit.
	if (context->count[0] < (inputlen << 3))
		context->count[1]++;
	context->count[1] += inputlen >> 29;

	//If new update content bigger than partlen,supplement the buffer.And transform.
	if (inputlen >= partlen) {
		memcpy(&context->buffer[index], input, partlen);
		MD5Transform(context->state, context->buffer);
		//After supplement,if residue more than 64byte,transform.
		for (i = partlen; i + 64 <= inputlen; i += 64)
			MD5Transform(context->state, &input[i]);
		index = 0;
	}
	else {
		i = 0;
	}
	memcpy(&context->buffer[index], &input[i], inputlen - i);
}

void MD5Final(MD5_CTX *context, unsigned char digest[16]) {
	unsigned int index = 0, padlen = 0;
	unsigned char bits[8];
	index = (context->count[0] >> 3) & 0x3F;
	//Make the last subgroup to 56byte,if last subgroup already more than 56byte,join a new subgroup to supplement. 
	padlen = (index < 56) ? (56 - index) : (120 - index);
	MD5Encode(bits, context->count, 8);  //Get context's size and put into char bits[];
	MD5Update(context, PADDING, padlen);  
	MD5Update(context, bits, 8); 
	MD5Encode(digest, context->state, 16);  //Get the result to digest[];
}

void MD5Encode(unsigned char *output, unsigned int *input, unsigned int len) {
	unsigned int i = 0, j = 0;
	while (j < len) {
		output[j] = input[i] & 0xFF;
		output[j + 1] = (input[i] >> 8) & 0xFF;
		output[j + 2] = (input[i] >> 16) & 0xFF;
		output[j + 3] = (input[i] >> 24) & 0xFF;
		i++;
		j += 4;
	}
}

void MD5Decode(unsigned int *output, unsigned char *input, unsigned int len) {
	unsigned int i = 0, j = 0;
	while (j < len) {
		output[i] = (input[j]) |
			(input[j + 1] << 8) |
			(input[j + 2] << 16) |
			(input[j + 3] << 24);
		i++;
		j += 4;
	}
}


void MD5Transform(unsigned int state[4], unsigned char block[64]) {
	unsigned int a = state[0];
	unsigned int b = state[1];
	unsigned int c = state[2];
	unsigned int d = state[3];
	unsigned int x[64];
	MD5Decode(x, block, 64);
	
	//4 rounds of calculation,each rounds have 16 times calculation.Totaly 46 times calculation.
	FF(a, b, c, d, x[0], 7, 0xd76aa478); /* 1 */
	FF(d, a, b, c, x[1], 12, 0xe8c7b756); /* 2 */
	FF(c, d, a, b, x[2], 17, 0x242070db); /* 3 */
	FF(b, c, d, a, x[3], 22, 0xc1bdceee); /* 4 */
	FF(a, b, c, d, x[4], 7, 0xf57c0faf); /* 5 */
	FF(d, a, b, c, x[5], 12, 0x4787c62a); /* 6 */
	FF(c, d, a, b, x[6], 17, 0xa8304613); /* 7 */
	FF(b, c, d, a, x[7], 22, 0xfd469501); /* 8 */
	FF(a, b, c, d, x[8], 7, 0x698098d8); /* 9 */
	FF(d, a, b, c, x[9], 12, 0x8b44f7af); /* 10 */
	FF(c, d, a, b, x[10], 17, 0xffff5bb1); /* 11 */
	FF(b, c, d, a, x[11], 22, 0x895cd7be); /* 12 */
	FF(a, b, c, d, x[12], 7, 0x6b901122); /* 13 */
	FF(d, a, b, c, x[13], 12, 0xfd987193); /* 14 */
	FF(c, d, a, b, x[14], 17, 0xa679438e); /* 15 */
	FF(b, c, d, a, x[15], 22, 0x49b40821); /* 16 */

	/* Round 2 */
	GG(a, b, c, d, x[1], 5, 0xf61e2562); /* 17 */
	GG(d, a, b, c, x[6], 9, 0xc040b340); /* 18 */
	GG(c, d, a, b, x[11], 14, 0x265e5a51); /* 19 */
	GG(b, c, d, a, x[0], 20, 0xe9b6c7aa); /* 20 */
	GG(a, b, c, d, x[5], 5, 0xd62f105d); /* 21 */
	GG(d, a, b, c, x[10], 9, 0x2441453); /* 22 */
	GG(c, d, a, b, x[15], 14, 0xd8a1e681); /* 23 */
	GG(b, c, d, a, x[4], 20, 0xe7d3fbc8); /* 24 */
	GG(a, b, c, d, x[9], 5, 0x21e1cde6); /* 25 */
	GG(d, a, b, c, x[14], 9, 0xc33707d6); /* 26 */
	GG(c, d, a, b, x[3], 14, 0xf4d50d87); /* 27 */
	GG(b, c, d, a, x[8], 20, 0x455a14ed); /* 28 */
	GG(a, b, c, d, x[13], 5, 0xa9e3e905); /* 29 */
	GG(d, a, b, c, x[2], 9, 0xfcefa3f8); /* 30 */
	GG(c, d, a, b, x[7], 14, 0x676f02d9); /* 31 */
	GG(b, c, d, a, x[12], 20, 0x8d2a4c8a); /* 32 */

	/* Round 3 */
	HH(a, b, c, d, x[5], 4, 0xfffa3942); /* 33 */
	HH(d, a, b, c, x[8], 11, 0x8771f681); /* 34 */
	HH(c, d, a, b, x[11], 16, 0x6d9d6122); /* 35 */
	HH(b, c, d, a, x[14], 23, 0xfde5380c); /* 36 */
	HH(a, b, c, d, x[1], 4, 0xa4beea44); /* 37 */
	HH(d, a, b, c, x[4], 11, 0x4bdecfa9); /* 38 */
	HH(c, d, a, b, x[7], 16, 0xf6bb4b60); /* 39 */
	HH(b, c, d, a, x[10], 23, 0xbebfbc70); /* 40 */
	HH(a, b, c, d, x[13], 4, 0x289b7ec6); /* 41 */
	HH(d, a, b, c, x[0], 11, 0xeaa127fa); /* 42 */
	HH(c, d, a, b, x[3], 16, 0xd4ef3085); /* 43 */
	HH(b, c, d, a, x[6], 23, 0x4881d05); /* 44 */
	HH(a, b, c, d, x[9], 4, 0xd9d4d039); /* 45 */
	HH(d, a, b, c, x[12], 11, 0xe6db99e5); /* 46 */
	HH(c, d, a, b, x[15], 16, 0x1fa27cf8); /* 47 */
	HH(b, c, d, a, x[2], 23, 0xc4ac5665); /* 48 */

	/* Round 4 */
	II(a, b, c, d, x[0], 6, 0xf4292244); /* 49 */
	II(d, a, b, c, x[7], 10, 0x432aff97); /* 50 */
	II(c, d, a, b, x[14], 15, 0xab9423a7); /* 51 */
	II(b, c, d, a, x[5], 21, 0xfc93a039); /* 52 */
	II(a, b, c, d, x[12], 6, 0x655b59c3); /* 53 */
	II(d, a, b, c, x[3], 10, 0x8f0ccc92); /* 54 */
	II(c, d, a, b, x[10], 15, 0xffeff47d); /* 55 */
	II(b, c, d, a, x[1], 21, 0x85845dd1); /* 56 */
	II(a, b, c, d, x[8], 6, 0x6fa87e4f); /* 57 */
	II(d, a, b, c, x[15], 10, 0xfe2ce6e0); /* 58 */
	II(c, d, a, b, x[6], 15, 0xa3014314); /* 59 */
	II(b, c, d, a, x[13], 21, 0x4e0811a1); /* 60 */
	II(a, b, c, d, x[4], 6, 0xf7537e82); /* 61 */
	II(d, a, b, c, x[11], 10, 0xbd3af235); /* 62 */
	II(c, d, a, b, x[2], 15, 0x2ad7d2bb); /* 63 */
	II(b, c, d, a, x[9], 21, 0xeb86d391); /* 64 */
	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
}


void GetAllFiles(wstring path, vector<wstring>& files)
{

	DWORD   hFile = 0;
	struct _wfinddata_t fileinfo;
	wstring p;

	// If path is c:\\windows,create a path like c:\\windows\\* to find subfile.
	// If _findfirst = -1,the path is file not a folder.else find the first file or folder under this path.
	if ((hFile = _wfindfirst(p.assign(path).append(L"\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib &  _A_SUBDIR))  // If the path is a folder,keep on search subfile. Else save the path.
			{
				if (lstrcmp(fileinfo.name, L".") != 0 && lstrcmp(fileinfo.name, L"..") != 0)
				{
					GetAllFiles(p.assign(path).append(L"\\").append(fileinfo.name), files);
				}
			}
			else
			{
				files.push_back(p.assign(path).append(L"\\").append(fileinfo.name));
			}

		} while (_wfindnext(hFile, &fileinfo) == 0);


	}
	else
	{
		files.push_back(p.assign(path));
	}
	_findclose(hFile);
}



int main(int argc, CHAR *argv[])
{
	CHAR    *SERVER_IP;
	u_short    PORT;
	TCHAR    *file_name;
	TCHAR    filenamebuf[FILE_NAME_MAX_SIZE+1];
	TCHAR    buffer[BUFFER_SIZE];
	CHAR     databuffer[BUFFER_SIZE];
	CHAR     md5buf[32];
	clock_t  sttm, fhtm;

	if (argc != 4)
	{
		printf("usage: \"%s\" <Server IP> <PORT> <FliePath>\n", argv[0]);
		return 1;
	}
	else
	{
		SERVER_IP = argv[1];
		PORT = atoi(argv[2]);
		USES_CONVERSION;
		file_name = A2W(argv[3]);

		// initialize socket dll  
		WSADATA wsaData;
		WORD socketVersion = MAKEWORD(2, 0);
		if (WSAStartup(socketVersion, &wsaData) != 0)
		{
			wprintf(L"Init socket dll error!");
			exit(1);
		}

		//Initialize sturct server_addr.
		sockaddr_in server_addr;
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.S_un.S_addr = inet_addr(SERVER_IP);
		server_addr.sin_port = htons(PORT);

		//get the file or the folder include subfiles
		wstring filePath = file_name;
		vector<wstring> files;
		GetAllFiles(filePath, files);
		int size = files.size();

		for (int i = 0; i < size; i++)
		{
			SOCKET c_Socket = socket(AF_INET, SOCK_STREAM, 0);
			if (SOCKET_ERROR == c_Socket)
			{
				wprintf(L"Create Socket Error!");
				system("pause");
				exit(1);
			}

			if (SOCKET_ERROR == connect(c_Socket, (LPSOCKADDR)&server_addr, sizeof(server_addr)))
			{
				wprintf(L"Can Not Connect To Server IP!\n");
				system("pause");
				exit(1);
			}
			sttm = clock();
			//Send filename.
			memcpy(filenamebuf, files[i].c_str(), lstrlen(files[i].c_str())*sizeof(TCHAR)+2);
			memset(buffer, 0, BUFFER_SIZE);
			memcpy(buffer, filenamebuf, lstrlen(filenamebuf)*sizeof(TCHAR) > BUFFER_SIZE ? BUFFER_SIZE : lstrlen(filenamebuf)*sizeof(TCHAR));

			USES_CONVERSION;
			if (send(c_Socket, W2A(buffer), BUFFER_SIZE, 0) < 0)
			{
				wprintf(L"Send File Name Failed\n");
				system("pause");
				exit(1);
			}

			//Initialize md5.
			unsigned char decrypt[16];
			MD5_CTX md5;
			MD5Init(&md5);
			CHAR *encrypt;

			FILE * fp = _wfopen(filenamebuf, L"rb");
			if (NULL == fp)
			{
				wprintf(L"File: %s Not Found\n", filenamebuf);
				memset(databuffer, 0, BUFFER_SIZE);
				send(c_Socket, databuffer, BUFFER_SIZE, 0);
			}
			else
			{
				//Get file's size and get md5.
				fseek(fp, 0, SEEK_END);
				DWORD len = ftell(fp);
				fseek(fp, 0, SEEK_SET);
				encrypt = (CHAR *)malloc(len*sizeof(CHAR));
				
				if (fread(encrypt, 1, len, fp) != len)
				{
					wprintf(L"READ ERROR!!!");
				}

				//Get md5
				MD5Init(&md5);
				MD5Update(&md5, (unsigned char*)encrypt, len);
				MD5Final(&md5, decrypt);
				
				memset(md5buf, 0, 32);
				for (int m = 0; m < 16; m++)
				{
					sprintf(md5buf, "%s%02x", md5buf, decrypt[m]);
				}
				printf("MD5:[%s]\n", md5buf);

				//Send flie's size
				CHAR filesize[16];
				memset(filesize, 0, 16);
				sprintf(filesize, "%d", len);
				printf("Filesize:[%s]\n", filesize);
				memset(databuffer, 0, BUFFER_SIZE);
				memcpy(databuffer, filesize, 16);
				if (send(c_Socket, databuffer, BUFFER_SIZE, 0) < 0)
				{
					wprintf(L"Send File size Failed\n");
					system("pause");
					exit(1);
				}
				

				//Send md5.
				memset(databuffer, 0, BUFFER_SIZE);
				memcpy(databuffer, md5buf, 32);
				if (send(c_Socket, databuffer, BUFFER_SIZE, 0) < 0)
				{
					wprintf(L"Send MD5 Failed\n");
					system("pause");
					exit(1);
				}

				memset(databuffer, 0, BUFFER_SIZE);
				int length = 0;
				fseek(fp, 0, SEEK_SET);

				// It cut file to many block with BUFFER_SIZE,for send. 
				DWORD nowsize = 0;
				while (nowsize < len)
				{
					if (len - nowsize < BUFFER_SIZE)
					{
						if ((length = fread(databuffer, 1, (len - nowsize), fp)) > 0)
						{
							if (send(c_Socket, databuffer, BUFFER_SIZE, 0) < 0)
							{
								wprintf(L"Send File: %s Failed\n", filenamebuf);
								break;
							}
							memset(databuffer, 0, BUFFER_SIZE);
						}
					}
					else
					{
						if ((length = fread(databuffer, 1, BUFFER_SIZE, fp)) > 0)
						{
							if (send(c_Socket, databuffer, BUFFER_SIZE, 0) < 0)
							{
								wprintf(L"Send File: %s Failed\n", filenamebuf);
								break;
							}
							memset(databuffer, 0, BUFFER_SIZE);
						}
					}
					nowsize = nowsize + length;
					
				}
				free(encrypt);
				fclose(fp);
				fhtm = clock();
				wprintf(L"File: %s Transfer Successful!\n", filenamebuf);
				wprintf(L"File Use [%fs] To Transfer\n", (DOUBLE)(fhtm-sttm)/CLOCKS_PER_SEC);
			}

			closesocket(c_Socket);
		}
	}
	WSACleanup();

	system("pause");
	return 0;
}
