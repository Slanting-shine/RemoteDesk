#include<iostream>
#include<WinSock2.h>
#include"net.h"

#pragma warning(disable : 4996)
#pragma warning(disable : 4700)
using namespace std;

int net_recv_all(SOCKET socket, void* buf, size_t len) {
    return recv(socket, (char*)buf, len, MSG_WAITALL);
}

/* preserverd for future Touch Moudule */
void MakePointerTouchInfoDown(POINTER_TOUCH_INFO& contact, int x, int y, int radius, int id, int orientation = 90, int pressure = 32000)
{
    ZeroMemory(&contact, sizeof(POINTER_TOUCH_INFO));
    contact.pointerInfo.pointerType = PT_TOUCH;
    contact.pointerInfo.ptPixelLocation.x = x;
    contact.pointerInfo.ptPixelLocation.y = y;
    contact.pointerInfo.pointerId = id;


    contact.touchFlags = TOUCH_FLAG_NONE;
    contact.pointerInfo.pointerFlags = POINTER_FLAG_DOWN | POINTER_FLAG_INRANGE | POINTER_FLAG_INCONTACT;
    contact.touchMask = TOUCH_MASK_NONE;
}

/* preserverd for future Touch Moudule */
void MakePointerTouchInfoMove(POINTER_TOUCH_INFO& contact, int x, int y, int radius, int id, int orientation = 90, int pressure = 32000)
{
    ZeroMemory(&contact, sizeof(POINTER_TOUCH_INFO));
    contact.pointerInfo.pointerType = PT_TOUCH;
    contact.pointerInfo.ptPixelLocation.x = x;
    contact.pointerInfo.ptPixelLocation.y = y;
    contact.pointerInfo.pointerId = id;


    contact.touchFlags = TOUCH_FLAG_NONE;
    contact.pointerInfo.pointerFlags = POINTER_FLAG_UPDATE | POINTER_FLAG_INRANGE | POINTER_FLAG_INCONTACT;
    contact.touchMask = TOUCH_MASK_NONE;
}

/* preserverd for future Touch Moudule */
void MakePointerTouchInfoUp(POINTER_TOUCH_INFO& contact, int x, int y, int radius, int id, int orientation = 90, int pressure = 32000)
{
    ZeroMemory(&contact, sizeof(POINTER_TOUCH_INFO));
    contact.pointerInfo.pointerType = PT_TOUCH;
    contact.pointerInfo.ptPixelLocation.x = x;
    contact.pointerInfo.ptPixelLocation.y = y;
    contact.pointerInfo.pointerId = id;


    contact.touchFlags = TOUCH_FLAG_NONE;
    contact.pointerInfo.pointerFlags = POINTER_FLAG_UP | POINTER_FLAG_INRANGE | POINTER_FLAG_INCONTACT;
    contact.touchMask = TOUCH_MASK_NONE;
}


DWORD CALLBACK ControlThread(LPVOID param) {

    //For Socket
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA data;
    if (WSAStartup(sockVersion, &data) != 0)
    {
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET)
    {
        printf("ServerSocket setup error !");
        return 0;
    }



    sockaddr_in sock_in;
    sock_in.sin_family = AF_INET;
    sock_in.sin_port = htons(7777);

    sock_in.sin_addr.S_un.S_addr = INADDR_ANY;

    if (bind(serverSocket, (LPSOCKADDR)&sock_in, sizeof(sock_in)) == SOCKET_ERROR)
    {
        printf("bind error !");
        return 1;
    }

    if (listen(serverSocket, 10) == SOCKET_ERROR) {
        printf("Listen error");
        return 1;
    }


    SOCKET clientSocket;
    sockaddr_in client_sin;
    int len = sizeof(client_sin);

    cout << "control thread wating" << endl;


    clientSocket = accept(serverSocket, (sockaddr*)&client_sin, &len);
    if (clientSocket == INVALID_SOCKET) {
        printf("Accept error");
        return 1;
    }

    printf("have a control connection\n");

   
    /*
    * 
    * preserverd for future Touch Moudule
    * 
    * InitializeTouchInjection(5, TOUCH_FEEDBACK_DEFAULT);
    * POINTER_TOUCH_INFO contacts[1];
    *
    */
   



    char msg[12];
    int action;
    int pointerId;
    float TouchPositonX;
    float TouchPositonY;
    int ret = 1;

    while (true)
    {

       int num = net_recv_all(clientSocket, msg, 12);

       if (num == 12) {
           memcpy(&action, &msg[0], 4);
          
           memcpy(&TouchPositonX, &msg[4], 4);
           memcpy(&TouchPositonY, &msg[8], 4);
           cout <<action<<"  "<< TouchPositonX << "     " << TouchPositonY << endl;
           int x = (int)TouchPositonX * (int)GetSystemMetrics(SM_CXSCREEN) / 1920;
           int y = (int)TouchPositonY * (int)GetSystemMetrics(SM_CYSCREEN) / 1140;

           switch (action) {
               
           case 0:
          
               SetCursorPos(x,y);
               mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN, (int)TouchPositonX * GetSystemMetrics(SM_CXSCREEN) / 1140,
                   (int)TouchPositonY / 1200 * GetSystemMetrics(SM_CYSCREEN), 0, 0);
             
               if (ret == 0) {
                   printf("code %d\n", GetLastError());

               }
               break;
           case 2:
               SetCursorPos(x, y);
              
               if (ret == 0) {
                   printf("code %d\n", GetLastError());

               }
               break;
           case 1:
           
               SetCursorPos(x, y);
               cout << "up" << endl;

               mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTUP, (int)TouchPositonX * GetSystemMetrics(SM_CXSCREEN) / 1920,
                   (int)TouchPositonY / 1200 * GetSystemMetrics(SM_CYSCREEN), 0, 0);
              
               if (ret == 0) {
                   printf("code %d\n", GetLastError());

               }
               break;

           }

       }

    }


}