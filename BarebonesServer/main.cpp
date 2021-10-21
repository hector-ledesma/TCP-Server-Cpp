#include <iostream>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

/*
	Tutorial will skip error checking. So I'll try to comment in which areas would require error checking, and what exactly could fail.
*/
void main() 
{
	// Initialize winsock
	WSADATA wsData;
	/*
		MAKEWORD is a MACRO equivalent to (x) | (y) << 8
		It uses LOBYTE and HIBYTE macros which extract the low- or hih-order byte from a WORD operand.

		Useful when packing two byte-sized values into a single 16-bit field.

		No longer used as much outside of winsockets.
	*/
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);

	if (wsOk != 0) {
		std::cerr << "Can't initialize winsock! QUitting" << std::endl;
		return;
	}
	// Create a socket
	/*
		AF stands for Address Family

		AF_INET is used to designate the type of addresses that your socket can communicate with to IPv4
	*/
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
	listen(listening, SOMAXCONN);

	// Wait for a connection
	sockaddr_in client;
	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(listening, (sockaddr*)&clientSocket, &clientSize);
	if (clientSocket == INVALID_SOCKET) {
		std::cerr << "Client returned an invalid socket! Quitting" << std::endl;
	}

	char host[NI_MAXHOST];		// Client's remote name
	char service[NI_MAXSERV];	// Service (i.e. port) the client is connected on

	ZeroMemory(host, NI_MAXHOST);	// same as memset(host, 0, NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&client, clientSize, host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		std::cout << host << " connected on port " << service << std::endl;
	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		std::cout << host << " connected on port " << ntohs(client.sin_port) << std::endl;
	}

	// Close listening socket
	closesocket(listening);

	// While loop: accept and echo message back to client
	char buf[4096];

	while (true)
	{
		ZeroMemory(buf, 4096);
		// Wait for client to send data
		int bytesReceived = recv(clientSocket, buf, 4096, 0);
		if (bytesReceived == SOCKET_ERROR)
		{
			std::cerr << "Error in recv(). Quitting" << std::endl;
			break;
		}

		if (bytesReceived == 0)
		{
			std::cout << "Client disconnected." << std::endl;
			break;
		}

		std::cout << std::string(buf, 0, bytesReceived) << std::endl;

		// Echo message back to client
		send(clientSocket, buf, bytesReceived + 1, 0);
	}

	// Close the socket
	closesocket(clientSocket);

	// Cleanup winsock
	WSACleanup();


}