#include <iostream>
#include <string>

#include "TCPListener.cpp"
#include "Qotd.h"

void Listener_MessageReceived(CTcpListener* listener, int client, std::string msg);

CQotd quotes("wisdom.txt");

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
	if (msg == "QUOTE")
	{
		listener->Send(client, quotes.GetRandomQuote());
	}
}