#include "Client.h"

void ClProc(SOCKET* Sender, std::string* Message, LPCVOID Content, UINT64 ContentSize)
{
	if (ContentSize > 0)
	{
		for (int i = 0; i < ContentSize; i++)
			putchar(*((char*)Content + i));
		putchar('\n');
	}
}

int main(int argc, char** argv)
{
	const char* ip = "192.168.1.5";
	const char* port = "25565";
	Client c;
	c.AddProcedure("Success", ClProc);
	c.ConnectToServer(ip, port);
	return c.StartSending();
}


