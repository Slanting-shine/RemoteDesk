#include "ScreenCapture.h"
#include <iostream>
#include <WinSock2.h>
#include "net.h"


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

#pragma comment(lib, "Ws2_32.lib")

#define UM_MSG1 WM_USER+1
#define UM_MSG2 WM_USER+2
#define MAX_BUFF_LENGTH 20

#pragma warning(disable : 4996)
#pragma warning(disable : 4700)

using namespace std;

int socket_send(SOCKET socket, const void* buf, size_t len) {
    int w = 0;
    while (len > 0) {
        w = send(socket, (const char*)buf, len, 0);
        //cout << "socket send byte  " << w << endl;
        if (w == -1) {
            cout << GetLastError() << endl;
            return -1;
        }
        len -= w;
        buf = (char*)buf + w;
    }
    return w;
}

void getFrame(AVFrame* frame) {
    HDC hdcMonitor;
    HDC hdcSource;
    HBITMAP hbmp;
    BYTE* bits= NULL;
    BITMAP bmp = { 0 };
    AVFrame* srcFrame;
    srcFrame = av_frame_alloc();
    SwsContext* convert_config_context;
    unsigned char* out_bufRgb;

    PVOID pbit = NULL;
    HBITMAP hBitmap = NULL;
    
    BITMAPINFO bmpinfo = { 0 };


    hdcMonitor = GetDC(NULL);
    hdcSource = CreateCompatibleDC(hdcMonitor);

    BITMAPINFO bi = { 0 };
    BITMAPINFOHEADER* bih = &bi.bmiHeader;
    bih->biSize = sizeof(BITMAPINFOHEADER);
    bih->biBitCount = 32;
    bih->biWidth = 1920;
    bih->biHeight = -1080;
    bih->biPlanes = 1;
    RGBQUAD* color = bi.bmiColors;

    out_bufRgb = (unsigned char*)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_RGB32
        , 1920
        , 1080
        , 1));


    av_image_fill_arrays(srcFrame->data, srcFrame->linesize, out_bufRgb, AV_PIX_FMT_RGB32, 1920, 1080, 1);

    hbmp = CreateDIBSection(hdcSource, &bi, DIB_RGB_COLORS, (void**)&bits, NULL, 0);
    SelectObject(hdcSource, hbmp);
    BitBlt(hdcSource, 0, 0, 1920, 1080, hdcMonitor, 0, 0, SRCCOPY | CAPTUREBLT);

    memcpy(out_bufRgb, bits, 1920 * 1080 * 4);
    //(const uint8_t*)&hbmp
    
    convert_config_context = sws_getContext(1920, 1080, AV_PIX_FMT_RGB32, 1920, 1080, AV_PIX_FMT_YUV420P,SWS_BILINEAR, NULL, NULL, NULL);

    if (!convert_config_context) {
        fprintf(stderr,
            "Impossible to create scale context for the conversion "
            "fmt:%s s:%dx%d -> fmt:%s s:%dx%d\n",
            av_get_pix_fmt_name(AV_PIX_FMT_RGB32), 1920, 1080,
            av_get_pix_fmt_name(AV_PIX_FMT_YUV420P), 1920, 1080);
        int ret;
        ret = AVERROR(EINVAL);
        return;
    }
    int a = 0;
    a = sws_scale(convert_config_context, (const unsigned char* const*)srcFrame->data,srcFrame->linesize,0,1080,frame->data,frame->linesize);

   
    //cout << a << endl;

    ReleaseDC(NULL, hdcSource);
    ReleaseDC(NULL, hdcMonitor);
    DeleteDC(hdcSource);
    DeleteDC(hdcMonitor);
    DeleteObject(hbmp);
    av_frame_free(&srcFrame);
    av_free(out_bufRgb);
    sws_freeContext(convert_config_context);
   
}

void encodeFrame(AVFrame* frame, AVCodecContext* context, AVPacket* packet,SOCKET clientSocket) {
    int ret;
    ret = avcodec_send_frame(context, frame);
    if (ret < 0) {
        fprintf(stderr, "Error sending a frame for encoding\n");
        exit(1);
    }
    //cout << "send frame" << endl;

    while (ret >= 0) {
        ret = avcodec_receive_packet(context, packet);
        //cout << "recive packet" << endl;
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
          // printf("return \n");
            return;
        }
        else if (ret < 0) {
            fprintf(stderr, "Error during encoding\n");
            exit(1);
        }

       // printf("Write packet %3lld (size=%5d)\n", packet->pts, packet->size);
       // int i = packet->size;
        
        BYTE* bytes = (BYTE*)malloc(packet->size + 12) ;
        memcpy(&bytes[0],(const void*)&packet->pts,8 );
        memcpy(&bytes[8], (const void*)&packet->size,4 );
        memcpy(&bytes[12], packet->data,packet->size);
        //PostThreadMessage(socketThreadId,UM_MSG2,(WPARAM)bytes,(LPARAM)(packet->size+12));
        
        socket_send(clientSocket, (const void*)bytes, packet->size+12);
        cout << "pts:"<<(long)packet->pts << "    size:" << (int)packet->size << endl;
      
        free(bytes);
        av_packet_unref(packet);
        


    }
}

void pushStream(AVFrame* frame, AVCodecContext* context, AVPacket* packet, SOCKET clientSocket) {
    int ret;
    ret = av_frame_make_writable(frame);

    if (ret < 0)
        exit(1);

    getFrame(frame);
   
    encodeFrame(frame,context,packet,clientSocket);
   
}


DWORD CALLBACK CaptureThread(LPVOID param)
{
   
    MSG msg;
    PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
    BOOL  bRet;
    AVCodecContext* context; 
    AVFrame* frame; 
    AVPacket* packet;

    const AVCodec* codec;
    int ret;
    

    codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!codec) {
        fprintf(stderr, "Codec not found\n");
        exit(1);
    }
    context = avcodec_alloc_context3(codec);
    if (!context) {
        fprintf(stderr, "Could not allocate video codec context\n");
        exit(1);
    }

    packet = av_packet_alloc();
    if (!packet)
        exit(1);

   
    /* put sample parameters */
    context->bit_rate = 40000000;

    /* resolution must be a multiple of two */
    context->width = 1920;
    context->height = 1080;
    context->codec_type = AVMEDIA_TYPE_VIDEO;

    /* frames per second */
    AVRational fuck1 = { 1,60 };
    AVRational fuck2 = { 60,1 };
    context->time_base = fuck1;
    context->framerate = fuck2;


    context->gop_size = 0 ;
    context->max_b_frames = 0;
    context->pix_fmt = AV_PIX_FMT_YUV420P;
   

       
    /* setting optimization*/
    av_opt_set(context->priv_data, "preset", "ultrafast", 0);
    av_opt_set(context->priv_data, "tune", "zerolatency", 0);

  

    /* open it */
    ret = avcodec_open2(context, codec, NULL);
    if (ret < 0) {
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }

    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }
    frame->format = context->pix_fmt;
    frame->width = context->width;
    frame->height = context->height;
  

    ret = av_frame_get_buffer(frame, 0);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate the video frame data\n");
        exit(1);
    }

    int i = 0;


    //For Socket
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA data;
     if (WSAStartup(sockVersion, &data) != 0)
     {
         exit(1);
     }

     SetupConnection();

    
     SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
     if (serverSocket == INVALID_SOCKET)
     {
         printf("ServerSocket setup error !");
         exit(1);
     }
     sockaddr_in sock_in;
     sock_in.sin_family = AF_INET;
     sock_in.sin_port = htons(8888);
     sock_in.sin_addr.S_un.S_addr = INADDR_ANY;

     if (bind(serverSocket, (LPSOCKADDR)&sock_in, sizeof(sock_in)) == SOCKET_ERROR)
     {
         printf("bind error !");
         exit(1);
     }

     if (listen(serverSocket, 10) == SOCKET_ERROR) {
         printf("Listen error");
         exit(1);
     }

     


     SOCKET clientSocket;
     sockaddr_in client_sin;
     int len = sizeof(client_sin);

     cout << "Capture thread wating" << endl;

     clientSocket = accept(serverSocket, (sockaddr*)&client_sin, &len);
     if (clientSocket == INVALID_SOCKET) {
         printf("Accept error");
         exit(1);
     }

     printf("have a  connection\n");

     while (bRet = PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE) == 0) {

         //·¢ËÍÖ¡Êý¾Ý
         frame->pts = i;
         pushStream(frame, context, packet, clientSocket);

         i++;
     }

      


    return true;
}

