#include "Client.h"

#pragma comment(lib, "Ws2_32.lib")

void usleep(__int64 usec)
{
	HANDLE timer;
	LARGE_INTEGER ft;

	ft.QuadPart = -(10 * usec); // Convert to 100 nanosecond interval, negative value indicates relative time

	timer = CreateWaitableTimerW(NULL, TRUE, NULL);
	SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
	WaitForSingleObject(timer, INFINITE);
	CloseHandle(timer);
}

bool Client::ConnectToServer(const char* ip, const char* port)
{
	int rv;
	if ((rv = getaddrinfo(ip, port, &hints, &servinfo)) != 0)
	{
		fwprintf(stderr, L"getaddrinof: %s\n", gai_strerrorW(rv));
		return false;
	}

	Server = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

	if (Server == INVALID_SOCKET)
	{
		closesocket(Server);
		perror("Can't create server socket");
		return false;
	}

	if (connect(Server, servinfo->ai_addr, static_cast<int>(servinfo->ai_addrlen)) != 0)
	{
		closesocket(Server);
		perror("Can't connect to server");
		return false;
	}

	this->ip = ip;
	this->port = port;

	return true;
}

UINT64 Client::RecieveContent(char** pBuffer, SOCKET* Sender, UINT64 Count)
{
	*pBuffer = (char*)malloc(Count);
	UINT64 numbytes = 0;
	int attemps = 0;

	while (numbytes < Count)
	{
		int r = recv(*Sender, *pBuffer + numbytes, static_cast<int>(Count - numbytes), 0);
		if (r == 0)
		{
			perror("Connection was closed");
			return -1;
		}
		else if (r == -1)
		{
			if (attemps++ == 5)
				return -1;
			usleep(10);
			continue;
		}

		numbytes += r;
	}

	printf("Recieved %lli\n", numbytes);

	return numbytes;
}

int Client::RecieveHeader(SOCKET* Sender)
{
	ClearBuffer();
	int numbytes = 0;
	int attemps = 0;

	while (numbytes < BufferSize)
	{
		int r = recv(*Sender, Buffer + numbytes, BufferSize - numbytes, MSG_WAITALL);
		if (r == 0)
		{
			perror("Connection was closed");
			return -1;
		}
		else if (r == -1)
		{
			if (attemps++ == 5)
				return -1;
			usleep(10);
			continue;
		}

		numbytes += r;
	}

	printf("Recieved %i\n", numbytes);

	return numbytes;
}

UINT64 Client::SendRequest(Request* Request)
{
	ClearBuffer();
	UINT64 numbytes = 0;
	// First send header
	memcpy(Buffer, Request->Header.c_str(), Request->Header.size() + 1);
	numbytes += Send(Buffer, &Server, BufferSize);

	// After header send content
	if (Request->Content)
		numbytes += Send(static_cast<char*>(Request->Content), &Server, Request->ContentSize);

	return numbytes;
}

UINT64 Client::Send(char* pBuffer, SOCKET* Sender, UINT64 Count)
{
	UINT64 numbytes = 0;
	int attemps = 0;

	while (numbytes < Count)
	{
		int r = send(*Sender, pBuffer + numbytes, static_cast<int>(Count - numbytes), 0);
		if (r == 0)
		{
			perror("Connection was closed");
			return -1;
		}
		else if (r == -1)
		{
			if (attemps++ == 5)
				return -1;
			usleep(10);
			continue;
		}

		numbytes += r;
	}

	printf("Data sended %lli\n", numbytes);

	return numbytes;
}

Client::Request* Client::CreateRequest(std::string ResponseMsg, Client::ContentType Type, LPVOID Content, UINT64 ContentSize)
{
	std::string Header = "Header: " + ResponseMsg + '\n';
	switch (Type)
	{
	case Client::ContentType::Data:
		Header += "Content-Type: Data\n";
		break;
	case Client::ContentType::File:
		Header += "Content-Type: File\n";
		break;
	case Client::ContentType::Message:
		Header += "Content-Type: Message\n";
		break;
	case Client::ContentType::Null:
	default:
		Header += "Content-Type: Null\n";
		break;
	}

	Header += "Content-Size: " + std::to_string(ContentSize) + '\n';

	Request* r = new Request();
	r->Header = Header;
	r->Content = Content;
	r->Type = Type;
	r->ContentSize = ContentSize;

	return r;
}

Client::Header* Client::GetHeader(char* Buffer, UINT32 BufferSize)
{
	Client::Header* Header = new Client::Header();
	std::string str;

	for (UINT32 i = 0; i < BufferSize; i++)
	{
		str += Buffer[i];
		if (str == "Header: ")
		{
			str = "";
			i++; // Skip space
			while (Buffer[i] != '\n' && Buffer[i] != '\0')
				Header->Message += Buffer[i++];
		}
		else if (str == "Content-Type: ")
		{
			str = "";
			i++;
			while (Buffer[i] != '\n' && Buffer[i] != '\0')
				str += Buffer[i++];
			if (str == "Data")
				Header->Type = ContentType::Data;
			else if (str == "File")
				Header->Type = ContentType::File;
			else if (str == "Message")
				Header->Type = ContentType::Message;
			else if (str == "Null")
				Header->Type = ContentType::Null;
			str = "";
		}
		else if (str == "Content-Size: ")
		{
			i++;
			str = "";
			while (Buffer[i] != '\n' && Buffer[i] != '\0')
				str += Buffer[i++];

			Header->ContentSize = std::stoull(str);
		}
	}
	return Header;
}

int Client::ExecuteCommand()
{
	if (RecieveHeader(&Server) < 0)
		return 0;

	printf("Recieved message:\n");
	char* str = Buffer;
	while (*str)
	{
		putchar(*str);
		str++;
	}

	Header* Header = GetHeader(Buffer, BufferSize);

	const auto key = Procedures.find(Header->Message);

	if (key == Procedures.end())
	{
		delete Header;
		putchar('\n');
		return -1;
	}

	if (Header->ContentSize > 0)
	{
		char* pBuffer;
		RecieveContent(&pBuffer, &Server, Header->ContentSize);

		Procedures[Header->Message](&Server, &Header->Message, pBuffer, Header->ContentSize);
		free(pBuffer);
	}
	else
		Procedures[Header->Message](&Server, &Header->Message, NULL, NULL);
	delete Header;
	putchar('\n');
	return 0;
}


int Client::StartSending()
{
	std::string line;

	getline(std::cin, line);

	while (line != "logout")
	{
		Request* Request = CreateRequest(line, ContentType::Message, NULL, NULL);
		SendRequest(Request);
		delete Request;

		if (line == "exit")
			return 0;

		ClearBuffer();
		ExecuteCommand();
		getline(std::cin, line);
	}
	return 0;
}

void Client::ClearBuffer()
{
	memset(Buffer, 0, BufferSize);
}

Client::Client()
{
	hints.ai_family = AF_INET; // IPv4
	hints.ai_socktype = SOCK_STREAM; // TCP

	WSADATA wsaData;

	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) 
		wprintf(L"Error at WSAStartup()\n");

	Buffer = static_cast<char*>(malloc(BufferSize));
}

bool Client::AddProcedure(std::string Header, std::function<void(SOCKET*, std::string*, LPCVOID, UINT64)> Procedure)
{
	const auto key = Procedures.find(Header);

	if (key == Procedures.end())
	{
		Procedures[Header] = Procedure;
		return true;
	}
	return false;
}