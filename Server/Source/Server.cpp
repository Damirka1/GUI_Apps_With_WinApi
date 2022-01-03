#include "Server.h"
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

int Server::StartServer(const char* port)
{
	printf("Creating server\n");
	this->Port = port;
	sockaddr_storage their_addr;
	socklen_t addr_size;
	addrinfo hints, *servinfo;
	SOCKET new_fd;
	int rv;
	Buffer = static_cast<char*>(malloc(BufferSize));

	memset(Buffer, 0, BufferSize);

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET; // Set ipv4
	hints.ai_socktype = SOCK_STREAM; // Set TCP type
	hints.ai_flags = AI_PASSIVE; // Set ip of host

	WSADATA wsaData;

	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		wprintf(L"Error at WSAStartup()\n");
		return 1;
	}

	if((rv = getaddrinfo(NULL, port, &hints, &servinfo)) != 0)
	{
		fwprintf(stderr, L"getaddrinfo: %s\n", gai_strerrorW(rv));
		return -1;
	}

	if((SockFd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == INVALID_SOCKET)
	{
		perror("Can't create server socket");
		return -1;
	}

	int opt = 1;

	if(setsockopt(SockFd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&opt), sizeof(opt)) == -1)
	{
		perror("setsockopt");
		return -1;
	}

	if(bind(SockFd, servinfo->ai_addr, static_cast<int>(servinfo->ai_addrlen)) == -1)
	{
		closesocket(SockFd);
		perror("bind error");
		return -1;
	}
	
	
	if(listen(SockFd, ListenCount) == -1)
	{
		perror("listen error");
		return -1;
	}

	Running = true;

	printf("Server started!\n");
	Thread = CreateThread(NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(&QueueProccessor), this, NULL, NULL);
	if(!Thread)
	{
		perror("Can't create queue thread");
		closesocket(SockFd);
		return -1;
	}

	while(Running)
	{
		addr_size = sizeof(their_addr);
		if((new_fd = accept(SockFd, (struct sockaddr*)&their_addr, &addr_size)) == SOCKET_ERROR)
		{
			usleep(100);
			continue;
		}

		for(int i = 0; i < ListenCount; i++)
		{
			if(Connections.c[i].fd == 0)
			{
				Connections.c[i].fd = new_fd;
				Connections.c[i].events = POLLIN;
				Connections.timeouts[i] = 0;
				printf("Added new connection to the queue\n");
				printf("New socket is %d, at %d\n", static_cast<int>(new_fd), i);
				break;
			}
		}
	}

	WaitForSingleObject(Thread, INFINITE);
	closesocket(SockFd);

	return 0;
}

bool Server::AddProcedure(std::string Header, std::function<void(Server*, SOCKET*, std::string*, LPCVOID, UINT64)> Procedure)
{
	const auto key = Procedures.find(Header);

	if (key == Procedures.end())
	{
		Procedures[Header] = Procedure;
		return true;
	}
	return false;
}

void Server::ClearBuffer()
{
	memset(Buffer, 0, BufferSize);
}


UINT64 Server::RecieveContent(char** pBuffer, SOCKET* Sender, UINT64 Count)
{
	*pBuffer = (char*)malloc(Count);
	UINT64 numbytes = 0;
	int attemps = 0;

	while(numbytes < Count)
	{
		int r = recv(*Sender, *pBuffer + numbytes, static_cast<int>(Count - numbytes), 0);
		if(r == 0)
		{
			perror("Connection was closed");
			return -1;
		}
		else if(r == -1)
		{
			if(attemps++ == 5)
				return -1;
			usleep(10);
			continue;
		}

		numbytes += r;
	}

	printf("Recieved %lli\n", numbytes);

	return numbytes;
}

int Server::RecieveHeader(SOCKET* Sender)
{
	ClearBuffer();
	int numbytes = 0;
	int attemps = 0;

	while (numbytes < BufferSize)
	{
		int r = recv(*Sender, Buffer + numbytes, BufferSize - numbytes, 0);
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

UINT64 Server::SendResponse(SOCKET* Sender, Response* Response)
{
	ClearBuffer();
	UINT64 numbytes = 0;
	// First send header
	memcpy(Buffer, Response->Header.c_str(), Response->Header.size() + 1);
	numbytes += Send(Buffer, Sender, BufferSize);

	// After header send content
	if(Response->Content)
		numbytes += Send(static_cast<const char*>(Response->Content), Sender, Response->ContentSize);

	ClearBuffer();
	return numbytes;
}

UINT64 Server::Send(const char* pBuffer, SOCKET* Sender, UINT64 Count)
{
	UINT64 numbytes = 0;
	int attemps = 0;

	while(numbytes < Count)
	{
		int r = send(*Sender, pBuffer + numbytes, static_cast<int>(Count - numbytes), 0);
		if(r == 0)
		{
			perror("Connection was closed");
			return -1;
		}
		else if(r == -1)
		{
			if(attemps++ == 5)
				return -1;
			usleep(10);
			continue;
		}

		numbytes += r;
	}

	printf("Data sended %lli\n", numbytes);

	return numbytes;
}


int Server::QueueProccessor(Server* Server)
{
	printf("Thread started!\n");
	while(Server->Running)
	{
		int poll_count = WSAPoll(Server->Connections.c, Server->ListenCount, 1);

		if (poll_count == SOCKET_ERROR)
		{
			//perror("poll");
			continue;
		}

		for(int i = 0; i < Server->ListenCount; i++)
		{
			if(Server->Connections.c[i].fd == 0)
				continue;

			pollfd* pfds = &Server->Connections.c[i];
			if(pfds->revents & POLLIN)
			{
				Server->Connections.timeouts[i] = 0;
				if(Server->RecieveHeader(&pfds->fd) > 0)
				{
					if(Server->ExecuteCommands(&pfds->fd) == -1)
						return 0;
				}
			}
			else
			{
				if(Server->Connections.timeouts[i]++ == 1000)
				{
					printf("Close connections %d, at %d\n", static_cast<int>(Server->Connections.c[i].fd), i);
					closesocket(Server->Connections.c[i].fd);
					Server->Connections.c[i].fd = 0;
					Server->Connections.timeouts[i] = 0;
				}

			}
		}
	}
	return 0;
}

Server::Header* Server::GetHeader(char* Buffer, UINT32 BufferSize)
{
	Server::Header* Header = new Server::Header();
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

Server::Response* Server::CreateResponse(std::string ResponseMsg, Server::ContentType Type, LPCVOID Content, UINT64 ContentSize)
{
	std::string Header = "Header: " + ResponseMsg + '\n';
	switch (Type)
	{
	case Server::ContentType::Data:
		Header += "Content-Type: Data\n";
		break;
	case Server::ContentType::File:
		Header += "Content-Type: File\n";
		break;
	case Server::ContentType::Message:
		Header += "Content-Type: Message\n";
		break;
	case Server::ContentType::Null:
	default:
		Header += "Content-Type: Null\n";
		break;
	}

	Header += "Content-Size: " + std::to_string(ContentSize) + '\n';

	Response* r = new Response();
	r->Header = Header;
	r->Content = Content;
	r->Type = Type;
	r->ContentSize = ContentSize;

	return r;
}

int Server::ExecuteCommands(SOCKET* Sender)
{
	if(strlen(Buffer) == 0)
		return -2;

	printf("Recieved message:\n");
	char* str = Buffer;
	while (*str)
	{
		putchar(*str);
		str++;
	}

	Header* Header = GetHeader(Buffer, BufferSize);

	if (Header->Message == "NOHEADER")
	{
		Response* r = CreateResponse("Invalid header", ContentType::Error, NULL, NULL);
		SendResponse(Sender, r);
		delete r;
		delete Header;
		putchar('\n');
		return 0;
	}

	const auto key = Procedures.find(Header->Message);

	if (key == Procedures.end())
	{
		Response* r = CreateResponse("Invalid request", ContentType::Error, NULL, NULL);
		SendResponse(Sender, r);
		delete r;
		delete Header;
		putchar('\n');
		return 0;
	}

	if (Header->ContentSize > 0)
	{
		char* pBuffer;
		RecieveContent(&pBuffer, Sender, Header->ContentSize);

		Procedures[Header->Message](this, Sender, &Header->Message, pBuffer, Header->ContentSize);
		free(pBuffer);
	}
	else
		Procedures[Header->Message](this, Sender, &Header->Message, NULL, NULL);

	delete Header;
	putchar('\n');
	return 0;
}
