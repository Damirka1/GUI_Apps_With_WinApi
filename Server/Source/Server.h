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


	enum class ContentType
	{
		Data,
		File,
		Message,
		Error,
		Null,
	};

	struct Header
	{
		std::string Message;
		ContentType Type;
		UINT64 ContentSize;
	};

	struct Response
	{
		std::string Header;
		LPVOID Content;
		ContentType Type;
		UINT64 ContentSize;
	};


private:
	SOCKET SockFd;
	bool Running;
	const char* Port;

	int ListenCount = LISTEN;
	int BufferSize = 1024; // 1 kb for commands
	char* Buffer;

	HANDLE Thread;

	std::unordered_map<std::string, std::function<void(SOCKET*, std::string*, LPCVOID, UINT64)>> Procedures;

private:
	static Response* CreateResponse(std::string ResponseMsg, Server::ContentType Type, LPVOID Content, UINT64 ContentSize);
	static Header* GetHeader(char* pBuffer, UINT32 BufferSize);
	static UINT64 Send(char* pBuffer, SOCKET* Sender, UINT64 Count);
	static UINT64 RecieveContent(char** pBuffer, SOCKET* Sender, UINT64 Count);
	UINT64 SendResponse(SOCKET* Sender, Response* Response);
	int RecieveHeader(SOCKET* Sender);
	void ClearBuffer();

	int ExecuteCommands(SOCKET* new_fd);
	static int QueueProccessor(Server* server);

public:
	int StartServer(const char* port);
	bool AddProcedure(std::string, std::function<void(SOCKET*, std::string*, LPCVOID, UINT64)>);
};
