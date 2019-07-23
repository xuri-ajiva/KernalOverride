#define WIN32_LEAN_AND_MEAN
#define _SRT_SECURE_NO_WARNINGS
#include "../KernalSharedData/SharedData.h"
#include "../KernalSharedData/SharedData.cpp"

#include <iostream>

#include <ctime>
#include <string>

#include <windows.h>

#include <winsock2.h>

#include <ws2tcpip.h>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

WSADATA wsaData;
SOCKET ConnectSocket = INVALID_SOCKET;
struct addrinfo *result = nullptr,
                *ptr = nullptr,
                hints;
const char* sendbuf = "this is a test";
char recvbuf[DEFAULT_BUFLEN];
int iResult, iSendResult;
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
	for (ptr = result; ptr != nullptr; ptr = ptr->ai_next)
	{
		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
		                       ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET)
		{
			printf("socket failed with error: %d\n", WSAGetLastError());
			WSACleanup();
			std::exit(-1); // or some other error code
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, static_cast<int>(ptr->ai_addrlen));
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


DWORD procID = NULL;
HWND hwnd = nullptr;

int __cdecl main(int /*argc*/, char** /*argv*/)
{
	system("cls");

	//Validate_parameters(argc, argv);

	Initialize_Winsock();

	//Resolve_server(argv[1]);
	Resolve_server("localhost");

	Attempt_connect();

	//// Send an initial buffer
	//Send(sendbuf);
	//Reseve();
	//cout << recvbuf << endl;
	// Receive until the peer closes the connection
	do
	{
		Send(StrToChar(string(ONLYDATA_S) + "1"));
		Reseve();
		
		Sleep_C();

		cout << "sending Window Name..." << endl;
		Send(StrToChar(string(FIND_WINDOW_S) + "*Unbenannt - Editor"));
		Reseve();
		cout << "Server replied: " << recvbuf << endl;
		hwnd = HWND(bufferToInt(recvbuf));

		Sleep_C();

		cout << "sending GetProcID..." << endl;
		string str1 = GET_WINDOW_THREAD_PROCESS_ID_S;
		Send(StrToChar(str1));
		Reseve();
		cout << "Server replied: " << recvbuf << endl;
		procID = DWORD(bufferToInt(recvbuf));

		Sleep_C();

		cout << "sending procID..." << endl;
		string str2 = HANDLE_S + std::to_string(procID);
		Send(StrToChar(str2));
		Reseve();
		cout << "Server replied: " << recvbuf << endl;

		Sleep_C();

		cout << "Sending Address..." << endl;
		string str3 = ADDRESS_S + std::to_string(DWORD(0x7FFF21B81580));
		Send(StrToChar(str3));
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
			int textLen = static_cast<int>(SendMessage(hwndEdit, WM_GETTEXTLENGTH, 0, 0));

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
						{
							std::cout << std::endl << str;
						}
						else
						{
							std::cout << buffer;
						}
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
