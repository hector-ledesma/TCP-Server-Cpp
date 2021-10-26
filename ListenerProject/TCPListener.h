#pragma once

#include <string>
#include <WS2tcpip.h>				//	Header file for Winsock funcitons
#pragma comment(lib, "ws2_32.lib")	//	Winsock library file

#define MAX_BUFFER_SIZE (49152)

//	Forward declaration of class
class CTcpListener;

//	TODO: Callback to data received
typedef void (*MessageReceivedHandler)(CTcpListener* listener, int socketId, std::string msg);
class CTcpListener
{
public:
	CTcpListener(std::string ipAddress, int port, MessageReceivedHandler handler);
	~CTcpListener();
	//	Send a message to the specified client
	void Send(int clientSocket, std::string msg);

	//	Initialize winsock
	bool Init();

	//	The main processing loop
	void Run();
		
	//	Receive loop
	//		Send back message
	//	Cleanup
	void Cleanup();

private:
	//	Create a socket
	SOCKET CreateSocket();

	//	Wait for a connection
	SOCKET WaitForConnection(SOCKET listening);

	std::string				m_ipAddress;
	int						m_port;
	MessageReceivedHandler	MessageReceived;
};