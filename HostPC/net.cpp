#include "net.h"
#include <WinSock2.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")
#define UM_MSG1 WM_USER+1
#define UM_MSG2 WM_USER+2
#define MAX_BUFF_LENGTH 20

#pragma warning(disable : 4996)
#pragma warning(disable : 4700)

using namespace std;

void SetupConnection( ) {

    SOCKET BroadcastRecvSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (BroadcastRecvSocket == INVALID_SOCKET) {
        printf("BroadcastRecvSocket setup error !");
    }

    SOCKADDR_IN sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(3779);
    sin.sin_addr.s_addr = INADDR_ANY;

    
    SOCKADDR_IN sin_from;
    sin_from.sin_family = AF_INET;
    sin_from.sin_port = htons(3779);
    sin_from.sin_addr.s_addr = INADDR_BROADCAST;

    
    bool bOpt = true;
    int err;
    setsockopt(BroadcastRecvSocket, SOL_SOCKET, SO_BROADCAST, (char*)&bOpt, sizeof(bOpt));

    
    err = bind(BroadcastRecvSocket, (SOCKADDR*)&sin, sizeof(SOCKADDR));
    if (SOCKET_ERROR == err)
    {
        err = WSAGetLastError();
        printf(" error! error code is %d/n", err);
        
    }

    int nAddrLen = sizeof(SOCKADDR);
    char buff[MAX_BUFF_LENGTH] = "";
    int nLoop = 0;
    
    printf("waiting for broadcast connection\n");
    int nSendSize = recvfrom(BroadcastRecvSocket, buff, MAX_BUFF_LENGTH, 0, (SOCKADDR*)&sin_from, &nAddrLen);



    printf("对方IP：%s ", inet_ntoa(sin_from.sin_addr));
    printf("对方PORT：%d ", ntohs(sin_from.sin_port));

    
    if (SOCKET_ERROR == nSendSize)
    {
        err = WSAGetLastError();
        printf(" error! error code is %d\n", err);
     
    }
    
    printf("Recv: %s\n", buff);

    int ret = sendto(BroadcastRecvSocket, buff, MAX_BUFF_LENGTH, 0, (sockaddr*)&sin_from, nAddrLen);

}











