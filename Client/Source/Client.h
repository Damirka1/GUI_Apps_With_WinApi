#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>

class Client
{
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

	struct Request
	{
		std::string Header;
		LPVOID Content;
		ContentType Type;
		UINT64 ContentSize;
	};

private:
	addrinfo hints, *servinfo;
	SOCKET Server;
	int BufferSize = 1024; // 1kb for commands
	char* Buffer = nullptr;
	const char* ip, *port;

	std::unordered_map<std::string, std::function<void(SOCKET*, std::string*, LPCVOID, UINT64)>> Procedures;

private:
	void ClearBuffer();
	int Reconnect();
	static UINT64 Send(char* pBuffer, SOCKET* Sender, UINT64 Count);
	UINT64 SendRequest(Request* Request);
	UINT64 RecieveContent(char** pBuffer, SOCKET* Sender, UINT64 Count);
	int RecieveHeader(SOCKET* Sender);
	int ExecuteCommand();
	Request* CreateRequest(std::string ResponseMsg, ContentType Type, LPVOID Content, UINT64 ContentSize);
	Header* GetHeader(char* Buffer, UINT32 BufferSize);

public:
	Client();

	bool ConnectToServer(const char* ip, const char* port);

	bool AddProcedure(std::string Header, std::function<void(SOCKET*, std::string*, LPCVOID, UINT64)> Procedure);

	int StartSending();

};