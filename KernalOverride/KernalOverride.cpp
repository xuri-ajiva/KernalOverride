//#define DEBUG
#define WIN32_LEAN_AND_MEAN
#define _SRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

WSADATA wsaData;
SOCKET ConnectSocket = INVALID_SOCKET;
struct addrinfo *result = NULL,
                *ptr = NULL,
                hints;
const char* sendbuf = "this is a test";
char recvbuf[DEFAULT_BUFLEN];
int iResult, iSendResult;
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


#pragma region ClientConnect

void Validate_parameters(int argc, char** argv)
{
	// Validate the parameters
	if (argc != 2)
	{
		printf("usage: %s server-name\n", argv[0]);

		std::exit(-1); // or some other error code
	}
}

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
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
}

void Resolve_server(const char* address)
{
	// Resolve the server address and port
	iResult = getaddrinfo(address, DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		std::exit(-1); // or some other error code
	}
}

void Attempt_connect()
{
	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{
		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
		                       ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET)
		{
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			std::exit(-1); // or some other error code
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET)
	{
		printf("Unable to connect to server!\n");
		WSACleanup();
		std::exit(-1); // or some other error code
	}
}

void Cleanup()
{
	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();
}

void Shutdown_connection()
{
	// shutdown the connection since no more data will be sent
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR)
	{
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		std::exit(-1); // or some other error code
	}
}

#pragma endregion

using namespace std;

char* StrToChar(const string str)
{
	char* finalstr = new char[str.length() + 1];

#pragma warning(disable : 4996)
	strcpy(finalstr, str.c_str());
#pragma warning(restore : 4996)
	return finalstr;
}

#define HANDLE_S "HANDLE:"
#define HANDLE_L 7
#define ADDRESS_S "ADDRESS:"
#define ADDRESS_L 8

int __cdecl main(int argc, char** argv)
{
	system("cls");

	//Validate_parameters(argc, argv);

	Initialize_Winsock();

	//Resolve_server(argv[1]);
	Resolve_server("localhost");

	Attempt_connect();

	// Send an initial buffer
	Send(sendbuf);
	Reseve();
	cout << recvbuf << endl;
	// Receive until the peer closes the connection
	do
	{
		cout << "finding HWND ... -> 0x";
		HWND hwnd = FindWindowA(nullptr, LPCSTR("*Unbenannt - Editor"));
		cout << hex << hwnd << endl;

		cout << "Get process ID ... -> 0x";
		DWORD procID;
		GetWindowThreadProcessId(hwnd, &procID);
		cout << hex << procID << endl;

		cout << "sending procID ..." << endl;
		string str1 = HANDLE_S + std::to_string(procID);
		Send(StrToChar(str1));
		Reseve();
		cout << "Server replied: " << recvbuf << endl;

		cout << "Sending Address ..." << endl;
		string str2 = ADDRESS_S + std::to_string(DWORD(0x633AFC));
		Send(StrToChar(str2));
		Reseve();
		cout << "Server replied: " << recvbuf << endl;

		system("pause");
	}
	while (iResult > 0);


	return 0;
}


#ifdef _WIN32
void dump_notepad_text()
{
	std::cout << "[DumpNotepadText]" << std::endl;

	std::cout << "- find Notepad window" << std::endl;
	HWND hwndNotepad = FindWindowA(nullptr, LPCSTR("*Unbenannt - Editor"));

	if (nullptr != hwndNotepad)
	{
		std::cout << "- find Edit control window" << std::endl;
		HWND hwndEdit = FindWindowExA(hwndNotepad, nullptr, LPCSTR("EDIT"), nullptr);

		if (nullptr != hwndEdit)
		{
			std::cout << "- get text length" << std::endl;
			int textLen = (int)SendMessage(hwndEdit, WM_GETTEXTLENGTH, 0, 0);

			if (0 < textLen)
			{
				std::cout << "- get text (up to 1024 chars, inc term null) : " << textLen << std::endl;
				const int bufferSize = 1024;
				char textBuffer[bufferSize] = "";
				SendMessage(hwndEdit, WM_GETTEXT, WPARAM(bufferSize), LPARAM(textBuffer));

				const auto str = " ->| ";

				std::cout << "[begin text]" << std::endl << str;
				for (auto buffer : textBuffer)
				{
					if (int(buffer) != 0)
					{
						if (buffer == '\n')
							std::cout << std::endl << str;
						else
							std::cout << buffer;
					}
				}
				std::cout << std::endl << "[end text]" << std::endl;
			}
			else
			{
				std::cerr << "? No text there! :-(" << std::endl;
			}
		}
		else
		{
			std::cerr << "? Huh? Can't find Edit control!?!" << std::endl;
		}
	}
	else
	{
		std::cerr << "? No notepads open! :-(" << std::endl;
	}
}
#endif


// Programm ausführen: STRG+F5 oder "Debuggen" > Menü "Ohne Debuggen starten"
// Programm debuggen: F5 oder "Debuggen" > Menü "Debuggen starten"
