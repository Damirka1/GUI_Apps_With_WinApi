#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string>
#include <functional>
#include <unordered_map>

#define LISTEN 20

class Server
{
	struct
	{
		struct pollfd c[LISTEN];
		int timeouts[LISTEN];
	} Connections;

private:
	SOCKET SockFd;
	bool Running;
	const char* Port;

	int ListenCount = LISTEN;
	int BufferSize = 1024; // 1 kb for commands
	char* Buffer;

	HANDLE Thread;

	std::unordered_map<std::string, std::function<void(Server*, SOCKET*, std::string*, LPCVOID, UINT64)>> Procedures;

public:
	enum class ContentType
	{
		Data,
		File,
		Message,
		Error,
		Null,
	};

	struct Response
	{
		std::string Header;
		LPCVOID Content;
		ContentType Type;
		UINT64 ContentSize;
	};

	struct Header
	{
		std::string Message;
		ContentType Type;
		UINT64 ContentSize;
	};

private:
	static Header* GetHeader(char* pBuffer, UINT32 BufferSize);
	static UINT64 Send(const char* pBuffer, SOCKET* Sender, UINT64 Count);
	static UINT64 RecieveContent(char** pBuffer, SOCKET* Sender, UINT64 Count);
	
	int RecieveHeader(SOCKET* Sender);
	void ClearBuffer();

	int ExecuteCommands(SOCKET* new_fd);
	static int QueueProccessor(Server* server);

public:
	int StartServer(const char* port);
	bool AddProcedure(std::string, std::function<void(Server*, SOCKET*, std::string*, LPCVOID, UINT64)>);
	static Response* CreateResponse(std::string ResponseMsg, Server::ContentType Type, LPCVOID Content, UINT64 ContentSize);
	UINT64 SendResponse(SOCKET* Sender, Response* Response);
};
