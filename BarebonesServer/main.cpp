#include <iostream>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

/*
	Tutorial will skip error checking. So I'll try to comment in which areas would require error checking, and what exactly could fail.
*/
void notMain() 
{
	// Initialize winsock
	/*
		WSADATA's is for the developer. It provides YOU information on how winsock was initialized.
		This information is filled out by your call to WSAStartup(version, dataPTR);
	*/
	WSADATA wsData;
	/*
		MAKEWORD is a MACRO equivalent to (x) | (y) << 8
		It uses LOBYTE and HIBYTE macros which extract the low- or hih-order byte from a WORD operand.

		Useful when packing two byte-sized values into a single 16-bit field.

		No longer used as much outside of winsockets.
	*/
	WORD ver = MAKEWORD(2, 2);

	/*
		WSAStartup will initialize all of the "behind the scene"/WinSock.dll and initialize the structs necessary to make the Socket related system calls.

		Every process that uses WiNSock must call WSAStartup within its own memory space.
	*/
	int wsOk = WSAStartup(ver, &wsData);

	if (wsOk != 0) {
		std::cerr << "Can't initialize winsock! QUitting" << std::endl;
		return;
	}
	// Create a socket
	/*
		socket() returns the file descriptor to the Socket that Windows is about to make available to you.

		File descriptors are like a "ticket" that your process can then use to utilize a resource. In this case, the resource is a Socket created by your system.

		---------------------------

		AF stands for Address Family

		AF_INET is used to designate the type of addresses that your socket can communicate with to IPv4

		AF_INET6 is to use IPv6.
	*/
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		std::cerr << "Can't create socket! Quitting" << std::endl;
		return;
	}

	// Bind the IP_ADDRESS and PORT to our socket
	/*
		The sockaddr_in struct will contain the information that winsock will use to make system calls.

		After a socket() call, the socket exists in a name space (address family) but has no address assigned to it.

		bind() assigns the address specified our sockaddr_in struct to our socket. This is traditionally referred to as "assigning a name to a socket".
	*/
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	/*
		htons() and ntohs() methods handle converting bytes between little and big endian.

		htons() = most significant byte first.
	*/
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY; // Could also use inet_pton...

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	// Tell winsock socket is for LISTENING
	/*
		listening() marks the socket as a PASSIVE socket. A socket that will be used to accept INCOMING connection requests using accept().

		The second parameter specifies the maximum length to which the queue of pending connections may grow.
	*/
	listen(listening, SOMAXCONN); // Error checking would be required here.

	// Wait for a connection
	/*
		The accept() call BLOCKS execution until there is a connection request present in the queue.

		Once there is a connection request, a socket is created to communicate with that specific connection. It creates a new FILE_DESCRIPTOR for this connected socket. This new socket is NOT in a listening state.

		Original socket is unaffected by this call. We simply use it to access its queue.

		The second argument will hold the socket.

		The third argument determines the size of the socket's address. If the size is too small, the address will be truncated.
	*/
	sockaddr_in client;
	int clientSize = sizeof(client);

	std::cout << "This is before the accept() call." << std::endl;
	SOCKET clientSocket = accept(listening, (sockaddr*)&clientSocket, &clientSize);
	if (clientSocket == INVALID_SOCKET) {
		std::cerr << "Client returned an invalid socket! Quitting" << std::endl;
	}
	std::cout << "This is AFTER the accept() call." << std::endl;

	/*
		ZeroMemory is a macro that fills a block of memory with 0s.
			|-- param1) Pointer to starting address of the block of mem.
			|-- param2) Size of the block of memory in bytes.

		----------

		getnameinfo() converts a socket address to a corresopnding host and service, in a protocol-independent manner.

		inet_ntop() converts the network address structure src in the af addres family into a character string.

		our host[] and service[] arrays are simply buffers. These buffers will be used by either getnameinfo() or inet_ntop() to fill in the data that they convert from the network.

	*/
	char host[NI_MAXHOST];		// Client's remote name
	char service[NI_MAXSERV];	// Service (i.e. port) the client is connected on

	ZeroMemory(host, NI_MAXHOST);	// same as memset(host, 0, NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&client, clientSize, host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		std::cout << "A.) ";
		std::cout << host << " connected on port " << service << std::endl;
	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		std::cout << "B.) ";
		std::cout << host << " connected on port " << ntohs(client.sin_port) << std::endl;
	}

	// Close listening socket
	/*
		We only care about connecting one client to our server, so we can close our listening socket after establishing a connected socket.
	*/
	closesocket(listening);

	// While loop: accept and echo message back to client
	/*
		recv(), recvfrom(), and recvmsg() calls are used to RECEIVE MESSAGES FROM A SOCKET. All three calls return the LENGTH of the message received on success, or -1 if failure. If message is larger than buffer, excess bytes may be discarded.

		Only difference between recv() and read() is that recv() has flags.

		recv(), like accept() waits for a message to be present unless it's specified as non-blocking.

		-----------

		The system calls send(), sendto(), and sendmsg() are used to transmit a message to another socket.

		send() call may only be used when the socket is in a CONNECTED state.



	*/
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

		std::cout << "Received: " << std::string(buf, 0, bytesReceived) << "of byte size: " << bytesReceived << std::endl;

		// Echo message back to client
		send(clientSocket, buf, bytesReceived + 1, 0);
	}

	// Close the socket
	closesocket(clientSocket);

	// Cleanup winsock
	WSACleanup();


}