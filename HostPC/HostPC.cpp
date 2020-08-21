
#include <iostream>
#include <Windows.h>
#include <process.h>
#include"ScreenCapture.h"
#include"net.h"

extern "C" {
#include "libavcodec\avcodec.h"  
#include "libavformat\avformat.h"  
#include "libavutil\channel_layout.h"  
#include "libavutil\common.h"  
#include "libavutil\imgutils.h"  
#include "libswscale\swscale.h" 
#include "libavutil\imgutils.h"      
#include "libavutil\opt.h"         
#include "libavutil\mathematics.h"      
#include "libavutil\samplefmt.h"
#include "libavutil\timestamp.h"
#include "libavutil\log.h"
#include "libavutil\time.h"
#include "libswresample\swresample.h" 
#include "libavutil\error.h"
}



DWORD ControlThreadID, CaptureThreadID;

DWORD CALLBACK ControlThread(LPVOID param);

DWORD CALLBACK CaptureThread(LPVOID param);



int main()
{
    HANDLE m_thread;
    m_thread = CreateThread(NULL, 0,CaptureThread, 0, 0, &CaptureThreadID);
    m_thread = CreateThread(NULL,0,ControlThread,0,0,&ControlThreadID);
    
   

    for (;;) {
        
        Sleep(1000); //retain for future functions

    }
   
  
    return 0;
}




