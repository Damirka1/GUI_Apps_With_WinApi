#include "Server.h"

void SvrProc(Server* This, SOCKET* Sender, std::string* Message, LPCVOID Content, UINT64 ContentSize)
{
	std::string str = "Hello from server!";
	printf("%s\n", str.c_str());
	Server::Response* R = This->CreateResponse("Success", Server::ContentType::Message, str.c_str(), str.size());
	This->SendResponse(Sender, R);
	delete R;
}

int main(int argc, char** argv)
{
	Server s;
	s.AddProcedure("Test.com", SvrProc);
	s.StartServer("25565");
}
