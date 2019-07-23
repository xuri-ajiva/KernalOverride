#include "pch.h"
#include "SharedData.h"
#include <string>
#include <iostream>
#include <winsock2.h>
#include <sstream>
#include <iomanip>


HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

static void Sleep_C()
{
	Sleep(DWORD(SLEEPDURA));
}

static char* StrToChar(const std::string& str)
{
	char* finalstr = new char[str.length() + 1];

#pragma warning(disable : 4996)
	// ReSharper disable CppDeprecatedEntity
	strcpy(finalstr, str.c_str());
	// ReSharper restore CppDeprecatedEntity
#pragma warning(restore : 4996)
	return finalstr;
}


int bufferToInt(char buff[])
{
	return int(std::atoi(StrToChar(std::string(buff)))); // NOLINT
}

#pragma region SendReseve


//WSADATA wsaData_L;
//SOCKET ConnectSocket_L= INVALID_SOCKET;
//char recvbuf_L[DEFAULT_BUFLEN];
//int iResult_L, iSendResult_L;
//int recvbuflen_L = DEFAULT_BUFLEN;

//int id = 0;

void Send_l(const char message[], int& id, SOCKET& ConnectSocket_L, int& iSendResult_L)
{
	iSendResult_L = send(ConnectSocket_L, message, static_cast<int>(strlen(message)), 0);
	if (iSendResult_L == SOCKET_ERROR)
	{
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket_L);
		WSACleanup();
		std::exit(-1); // or some other error code
	}

#ifdef DEBUG
	std::stringstream basic_stringstream;
	basic_stringstream << std::setw(5) << std::setfill(' ') << iSendResult_L;
	std::string iSendResult_L_S = basic_stringstream.str();

	std::stringstream IDs;
	IDs << std::setw(5) << std::setfill(' ') << id;
	std::string IDString = IDs.str();

	SetConsoleTextAttribute(hConsole, 14);
	std::cout << IDString << " | " << iSendResult_L_S << " B |   send   | " << message << std::endl;
	SetConsoleTextAttribute(hConsole, 7);
#endif

	id++;
}

char* Reseve_l(int& id, SOCKET& ConnectSocket_L, int& iSendResult_L, int& iResult_L, int& recvbuflen_L,
               char recvbuf_L[])
{
	std::fill_n(recvbuf_L, recvbuflen_L, 0);
	iResult_L = recv(ConnectSocket_L, recvbuf_L, recvbuflen_L, 0);

	if (iResult_L == 0)
	{
		printf("Connection closing...\n");
	}
	else if (iResult_L < 0)
	{
		printf("recv failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket_L);
		WSACleanup();
		std::exit(-1); // or some other error code
	}

#ifdef DEBUG
	std::stringstream basic_stringstream;
	basic_stringstream << std::setw(5) << std::setfill(' ') << iResult_L;
	std::string iResult_L_S = basic_stringstream.str();

	std::stringstream IDs;
	IDs << std::setw(5) << std::setfill(' ') << id;
	std::string IDString = IDs.str();

	SetConsoleTextAttribute(hConsole, 6);
	std::cout << IDString << " | " << iResult_L_S << " B | received | " << recvbuf_L << std::endl;
	SetConsoleTextAttribute(hConsole, 7);
#endif

	id++;
	return recvbuf_L;
}

#pragma endregion
