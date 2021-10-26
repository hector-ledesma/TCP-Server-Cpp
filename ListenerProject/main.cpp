#include <iostream>
#include <string>

#include "TCPListener.h"

void Listener_MessageReceived(CTcpListener* listener, int client, std::string msg);

void main()
{
	CTcpListener server("127.0.0.1", 54010, Listener_MessageReceived);

	if (server.Init())
	{
		server.Run();
	}
}

void Listener_MessageReceived(CTcpListener* listener, int client, std::string msg)
{
	listener->Send(client, msg);
}