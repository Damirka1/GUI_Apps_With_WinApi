#define  _CRT_SECURE_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <fcntl.h>
#include <iostream>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

struct addrinfo hints, * servinfo;
SOCKET SockFd = 0;
int buffer_command_size = 2048; // 2kb for commands
int buffer_size = 131072; // 128 kb for data
char* buffer;

int Connect(SOCKET* sockfd, struct addrinfo* servinfo)
{
	if((*sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1)
	{
		closesocket(*sockfd);
		perror("Can't create server socket");
		return -1;
	}

	if(connect(*sockfd, servinfo->ai_addr, static_cast<int>(servinfo->ai_addrlen)) == -1)
	{
		closesocket(*sockfd);
		perror("Can't connect to server");
		return -1;
	}

	return 0;
}

void ClearBuffer(void* buffer, int* buffer_size)
{
	memset(buffer, 0, *buffer_size);
}

int Recieve(SOCKET* sockfd, int count, char* buffer, int* buffer_size)
{
	ClearBuffer(buffer, buffer_size);

	int numbytes = recv(*sockfd, buffer, count, 0);
	int attemps = 0;
	if(numbytes <= 0)
	{
		perror("Can't recieve data");
		return -1;
	}

	while(numbytes < count)
	{
		int r = recv(*sockfd, buffer + numbytes, *buffer_size - numbytes, 0);
		if(r == -1)
		{
			perror("Can't recieve data in cycle");
			return -1;
		}
		else if(r == 0)
		{
			if(attemps++ == 1000)
				return -1;
			Sleep(1);
			continue;
		}
		numbytes += r;
	}
	printf("end recv %d\n", numbytes);
	return numbytes;
}

int Send(SOCKET* sockfd, int count, char* buffer, int* buffer_size)
{
	if(count > *buffer_size)
		count = *buffer_size;

	int numbytes = send(*sockfd, buffer, count, 0);
	int attemps = 0;
	if(numbytes < 0)
	{
		perror("Can't send data");
		Connect(&SockFd, servinfo);
		printf("Reconnecting to server...\n");
	}
	printf("count = %d\n", numbytes);

	while(numbytes < count)
	{
		int r = send(*sockfd, buffer + numbytes, *buffer_size - numbytes, 0);
		if(r == -1)
		{
			perror("Can't send data in cycle");
			return -1;
		}
		else if(r == 0)
		{
			if(attemps++ == 3)
				return -1;
			Sleep(1);
			continue;
		}
		numbytes += r;
	}
	printf("Data sended, count = %d\n", numbytes);
	ClearBuffer(buffer, buffer_size);
	return numbytes;
}

void ExecuteCommands()
{
	ClearBuffer(buffer, &buffer_size);
	Recieve(&SockFd, buffer_command_size, buffer, &buffer_size);

	if(strlen(buffer) == 0)
		return;
	
	printf("Response from server is %s\n", buffer);

	char* saveptr;
	char* token = strtok_s(buffer, " ", &saveptr);

	if(strcmp(buffer, "file") == 0)
	{
		printf("Downloading file\n");
		char* fsize = buffer + strlen(token) + 1;
		int filesize = 0;
		memcpy(&filesize, fsize, sizeof(long long));
		printf("Size of file = %i\n", filesize);

		//void* f = malloc(filesize);

		FILE* file;
		fopen_s(&file, "get", "wb");

		int it = 0;
		int readsize = filesize;

		if(readsize > buffer_size)
			readsize = buffer_size;
		
		while(it != filesize)
		{
			if(it + readsize > filesize)
			{
				readsize = filesize - it;
				if(readsize < 0)
					readsize = filesize;
			}

			Recieve(&SockFd, readsize, buffer, &buffer_size);
			//memcpy(f + it, buffer, buffer_size);
			fwrite(buffer, readsize, 1, file);
			it += readsize;
		}

		//fwrite(f, filesize, 1, file);
		
		fclose(file);
		printf("File downloaded!\n");
	}
}

int main(int argc, char** argv)
{
	int rv;
	const char* ip = "192.168.1.5";
	const char* port = "25565";

	buffer = static_cast<char*>(malloc(buffer_size));
	
	memset(buffer, 0, buffer_size);
	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_INET; // IPv4
	hints.ai_socktype = SOCK_STREAM; // TCP

	WSADATA wsaData;

	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		wprintf(L"Error at WSAStartup()\n");
		return 1;
	}

	if((rv = getaddrinfo(ip, port, &hints, &servinfo)) != 0)
	{
		fwprintf(stderr, L"getaddrinof: %s\n", gai_strerrorW(rv));
		return -1;
	}

	Connect(&SockFd, servinfo);

	std::string line;
	
	getline(std::cin, line);

	while(line != "logout")
	{
		memcpy(buffer, line.c_str(), line.size() + 1);
		Send(&SockFd, buffer_command_size, buffer, &buffer_size);

		if(line != "exit")
			return 0;
		
		ExecuteCommands();
		memset(buffer, 0, buffer_size);
		getline(std::cin, line);
	}
}


