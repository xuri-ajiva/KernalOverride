//#define DEBUG
#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include "KernalServer.h"

#include <array>

#include <iostream>

#include <stdio.h>

#include <cstdlib>
#include <windows.h>

#include <winsock2.h>

#include <ws2tcpip.h>
#include <string>


// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

WSADATA wsaData;
int iResult;

SOCKET ListenSocket = INVALID_SOCKET;
SOCKET ConnectSocket = INVALID_SOCKET;

struct addrinfo* result = nullptr;
struct addrinfo hints;

int iSendResult;
char recvbuf[DEFAULT_BUFLEN];
int recvbuflen = DEFAULT_BUFLEN;

#pragma region SendReseve

void Send(const char message[])
{
	iSendResult = send(ConnectSocket, message, (int)strlen(message), 0);
	if (iSendResult == SOCKET_ERROR)
	{
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		std::exit(-1); // or some other error code
	}
#ifdef DEBUG
	std::cout << iSendResult << " Bytes |   sent   | " << message << std::endl;
#endif
}

void Reseve()
{
	std::fill_n(recvbuf, recvbuflen, 0);
	iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);

	if (iResult == 0)
	{
		printf("Connection closing...\n");
	}
	else if (iResult < 0)
	{
		printf("recv failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		std::exit(-1); // or some other error code
	}
#ifdef DEBUG
	std::cout << iResult << " Bytes | received | " << recvbuf << std::endl;
#endif
}

#pragma endregion

#pragma region ServerInitAndConnect

void Initialize_Winsock()
{
	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		printf("WSAStartup failed with error: %d\n", iResult);
		std::exit(-1); // or some other error code
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
}

void Resolve_server()
{
	// Resolve the server address and port
	iResult = getaddrinfo(nullptr, DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		std::exit(-1); // or some other error code
	}
}

void Create_SOCKET()
{
	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET)
	{
		printf("socket failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		std::exit(-1); // or some other error code
	}
}

void Setup_TCP()
{
	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, static_cast<int>(result->ai_addrlen));
	if (iResult == SOCKET_ERROR)
	{
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		std::exit(-1); // or some other error code
	}

	freeaddrinfo(result);

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR)
	{
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		std::exit(-1); // or some other error code
	}
}

void Accept_client()
{
	// Accept a client socket
	ConnectSocket = accept(ListenSocket, nullptr, nullptr);
	if (ConnectSocket == INVALID_SOCKET)
	{
		printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		std::exit(-1); // or some other error code
	}
	// No longer need server socket
	closesocket(ListenSocket);
}

void Shutdown_connection()
{
	// shutdown the connection since we're done
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR)
	{
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		std::exit(-1); // or some other error code
	}
	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();
}

#pragma endregion


#define HANDLE_S "HANDLE:"
#define HANDLE_L 7
#define ADDRESS_S "ADDRESS:"
#define ADDRESS_L 8

DWORD procID;
DWORD toReed;
HANDLE handle;

using namespace std;

char* StrToChar(const string str)
{
	char* finalstr = new char[str.length() + 1];

#pragma warning(disable : 4996)
	strcpy(finalstr, str.c_str());
#pragma warning(restore : 4996)
	return finalstr;
}

int __cdecl main()
{
	system("cls");

	Initialize_Winsock();

	Resolve_server();

	Create_SOCKET();

	Setup_TCP();

	Accept_client();

	// Receive until the peer shuts down the connection
	do
	{
		while (true)
		{
			Reseve();
			string s = std::string(recvbuf);
			if (s.substr(0, HANDLE_L) != HANDLE_S)
			{
				Send("Pleas Send Handle!");
				continue;
			}
			else
			{
				procID = DWORD(atoi(s.substr(HANDLE_L).c_str()));
				cout << "Opening ProcessID 0x" << hex << procID << std::dec <<
					" with \"PROCESS_ALL_ACCESS\" ..." << endl;

				handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);

				if (handle == nullptr)
				{
					cout << "Process nicht gefunden!" << endl;
					Send("Process nicht gefunden!");
					Sleep(3000);
					Shutdown_connection();
				}
				cout << "Process Handle: " + to_string(int(handle)) << endl;
				Send(StrToChar("Process Handle: " + to_string(int(handle))));
				break;
			}
		}
		while (true)
		{
			Reseve();
			string s = std::string(recvbuf);
			if (s.substr(0, ADDRESS_L) != ADDRESS_S)
			{
				Send("Send Address to read");
				continue;
			}
			else
			{
				toReed = DWORD(atoi(s.substr(ADDRESS_L).c_str()));
				cout << "Reading in Process 0x" << hex << procID << std::dec << " of process handle " << hex <<
					handle << dec << " at 0x" <<hex << toReed << " ..." << endl;

				float y;
				ReadProcessMemory(handle, LPVOID(toReed), &y, sizeof(y), 0);

				cout << "Read at 0x" << hex << toReed << dec << " value: " << y << endl;
				Send(StrToChar(("Read at 0x" + to_string(int(toReed))) + " value:" + to_string(y)));
				break;
			}
		}

		//Reseve();


		system("pause");
	}
	while (iResult > 0);

	Shutdown_connection();

	return 0;
}
