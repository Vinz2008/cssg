#include "windows.h"
#include "../src/windows.h"
#include "webserver.h"

#ifdef _WIN32

#include <winsock2.h>
#include <Ws2tcpip.h>



void WIN32InitSocket(){
    WSADATA wsa_data;
    WSAStartup(MAKEWORD(1,1), &wsa_data);
}


void WIN32QuitSocket(){
    WSACleanup();
}


void WIN32CloseSocket(SOCKET_TYPE sock){
    shutdown(sock, SD_BOTH);
    close(sock);
}

#endif