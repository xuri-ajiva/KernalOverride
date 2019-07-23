//#define DEBUG
#undef UNICODE
#include "../KernalSharedData/SharedData.h"
#include "../KernalSharedData/SharedData.cpp"

#define WIN32_LEAN_AND_MEAN

#include "KernalServer.h"

#include <array>
#include <ctime>
#include <iostream>

#include <cstdlib>
#include <windows.h>

#include <winsock2.h>

#include <ws2tcpip.h>
#include <string>


// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

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

int id = 0;

void Send(const char message[])
{
	Send_l(message, id, ConnectSocket, iSendResult);
}

void Reseve()
{
	const auto res = Reseve_l(id, ConnectSocket, iSendResult, iResult, recvbuflen, recvbuf);
	// ReSharper disable once CppDeprecatedEntity
	strcpy(recvbuf, res);
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

DWORD procID;
DWORD toReed;
HANDLE handle;
HWND hwnd;

BOOLEAN ONLYDATA = true;

using namespace std;

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
		Reseve();
		string s = std::string(recvbuf);

		if (s.substr(0, ONLYDATA_L) == ONLYDATA_S)
		{
			ONLYDATA = s.substr(ONLYDATA_L, 1) == "1";

			Sleep(DWORD(30));

			if (ONLYDATA)
				Send(StrToChar("1"));
			else
				Send(StrToChar("Set ONLYDATA to 0"));
		}
		else if (s.substr(0, HANDLE_L) == HANDLE_S)
		{
			procID = DWORD(atoi(s.substr(HANDLE_L).c_str()));
			cout << "Opening ProcessID " << procID << std::dec <<
				" with \"PROCESS_ALL_ACCESS\" ..." << endl;

			handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);

			Sleep(DWORD(30));

			if (handle == nullptr)
			{
				cout << "Process nicht gefunden!" << endl;
				if (ONLYDATA)
					Send(ERRORVAULE);
				else
					Send("Process nicht gefunden!");
			}
			else
			{
				cout << "Process Handle: " + to_string(int(handle)) << endl;
				if (ONLYDATA)
					Send(StrToChar(to_string(int(handle))));
				else
					Send(StrToChar("Process Handle: " + to_string(int(handle))));
			}
		}
		else if (s.substr(0, ADDRESS_L) == ADDRESS_S)
		{
			toReed = DWORD(atoi(s.substr(ADDRESS_L).c_str()));
			cout << "Reading in Process " << procID << " at " << toReed << " ..." << endl;

			float y;
			ReadProcessMemory(handle, LPVOID(toReed), &y, sizeof(y), 0);

			Sleep(DWORD(30));

			cout << "Read at " << toReed << " value: " << y << endl;
			if (ONLYDATA)
				Send(StrToChar(to_string(y)));
			else
				Send(StrToChar(("Read at " + to_string(int(toReed))) + " value:" + to_string(y)));
		}
		else if (s.substr(0, FIND_WINDOW_L) == FIND_WINDOW_S)
		{
			hwnd = FindWindowA(nullptr, LPCSTR(StrToChar(s.substr(FIND_WINDOW_L))));

			Sleep(DWORD(30));

			if (hwnd == nullptr)
			{
				cout << "Window Dose Not Exist" << endl;
				if (ONLYDATA)
					Send(ERRORVAULE);
				else
					Send("Window Dose Not Exist");
			}
			else
			{
				cout << "FindWindowA reviews hWnd (rep): " << hwnd << endl;
				if (ONLYDATA)
					Send(StrToChar(to_string(int(1))));
				else
					Send(StrToChar("FindWindowA reviews hWnd: " + int(GlobalHandle(hwnd))));
			}
		}
		else if (s.substr(0, GET_WINDOW_THREAD_PROCESS_ID_L) == GET_WINDOW_THREAD_PROCESS_ID_S)
		{
			//if (HWND(StrToChar(s.substr(GET_WINDOW_THREAD_PROCESS_ID_L))) == hwnd)
			//{
			GetWindowThreadProcessId(hwnd, &procID);

			Sleep(DWORD(30));

			if (procID == NULL)
			{
				cout << "Process Id Dose Not Exist!" << endl;
				if (ONLYDATA)
					Send(ERRORVAULE);
				else
					Send(StrToChar("Process Id Dose Not Exist!"));
			}
			else
			{
				cout << "hWnd " << hwnd << " resolved to Process id " << procID << endl;
				if (ONLYDATA)
					Send(StrToChar(to_string(int(procID))));
				else
					Send(StrToChar(
						"hWnd " + to_string(int(hwnd)) + " resolved to Process id " + to_string(int(procID))));
			}
			//}
			//else
			//{
			//	cout << "hWnd dose not march!" << endl;
			//	if (ONLYDATA)
			//		Send(ERRORVAULE);
			//	else
			//		Send(StrToChar("hWnd dose not march!"));
			//}
		}
		else
		{
			Sleep(DWORD(30));

			Send(StrToChar("Send known Command ->[" + s + "]"));
		}
	}
	while (iResult > 0);

	Shutdown_connection();
	system("pause");
	return 0;
}
