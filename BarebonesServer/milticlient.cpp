#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <sstream>

#pragma comment (lib, "ws2_32.lib")

void main()
{
	// Initialize winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);
	int wsOk = WSAStartup(ver, &wsData);

	if (wsOk != 0) {
		std::cerr << "Can't initialize winsock! QUitting" << std::endl;
		return;
	}
	// Create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		std::cerr << "Can't create socket! Quitting" << std::endl;
		return;
	}

	// Bind the IP_ADDRESS and PORT to our socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY; // Could also use inet_pton...

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	// Tell winsock socket is for LISTENING
	listen(listening, SOMAXCONN); // Error checking would be required here.

	fd_set master;
	FD_ZERO(&master);

	FD_SET(listening, &master);

	std::string welcomeMsg = "Welcome to the Awesome Chat Server!";
	while (true)
	{
		fd_set copy = master;

		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

		for (int i = 0; i < socketCount; i++)
		{
			SOCKET sock = copy.fd_array[i];
			if (sock == listening)
			{
				//	Accept a new connection.
				SOCKET client = accept(listening, nullptr, nullptr);

				//	Add the new connection to the list of connected clients.
				FD_SET(client, &master);

				//	Send a welcome message to the connected client.
				send(client, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);

			}
			else
			{
				char buf[4096];
				ZeroMemory(buf, 4096);
				//	Accept a new message.
				int bytesIn = recv(sock, buf, 4096, 0);
				if (bytesIn <= 0)
				{
					//	Drop the client
					closesocket(sock);
					FD_CLR(sock, &master);
				}
				else
				{
					//	Send message to OTHER clients, and NOT the LISTENING socket.
					for (int i = 0; i < master.fd_count; i++)
					{
						SOCKET outSock = master.fd_array[i];
						if (outSock != listening && outSock != sock)
						{
							std::ostringstream ss;
							ss << "Socket #" << sock << ": " << buf << "\r\n";
							std::string strOut = ss.str();

							send(outSock, strOut.c_str(), strOut.size() + 1, 0);
						}
					}
				}
				
				
			}


		}
	}

	// Cleanup winsock
	WSACleanup();

}