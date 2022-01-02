#include "Server.h"

void SvrProc(SOCKET* Sender, std::string* Message, LPCVOID Content, UINT64 ContentSize)
{
	printf("Hello, world!\n");
}

int main(int argc, char** argv)
{
	Server s;
	s.AddProcedure("Test.com", SvrProc);
	s.StartServer("25565");
}
