#define DEBUG
#define WIN32_LEAN_AND_MEAN
#define _SRT_SECURE_NO_WARNINGS
#undef UNICODE

#include <string>
#include <array>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")
// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define ERRORVAULE "-1"

#define HANDLE_S "HANDLE:"
#define HANDLE_L 7
#define ADDRESS_S "ADDRESS:"
#define ADDRESS_L 8
#define FIND_WINDOW_S "FINDWINDOW:"
#define FIND_WINDOW_L  11
#define ONLYDATA_S "ONLYDATA:"
#define ONLYDATA_L 9
#define GET_WINDOW_THREAD_PROCESS_ID_S "GWTPI:"
#define GET_WINDOW_THREAD_PROCESS_ID_L 6


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"
#define SLEEPDURA 100

static int bufferToInt(char buff[]);
static void Sleep_C();
static char* StrToChar(const std::string& str);
