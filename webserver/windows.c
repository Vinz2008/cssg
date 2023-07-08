#include "windows.h"
#include "../windows.h"
#include "webserver.h"

#ifdef _WIN32
#include <winsock2.h>
#include <Ws2tcpip.h>
#endif


void WIN32InitSocket(){
#ifdef _WIN32
    WSADATA wsa_data;
    WSAStartup(MAKEWORD(1,1), &wsa_data);
#endif
}


void WIN32QuitSocket(){
#ifdef _WIN32
    WSACleanup();
#endif
}


void WIN32CloseSocket(SOCKET_TYPE sock){
#ifdef _WIN32
    shutdown(sock, SD_BOTH);
    close(sock);
#endif
}