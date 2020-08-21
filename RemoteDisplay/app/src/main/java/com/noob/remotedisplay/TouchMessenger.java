package com.noob.remotedisplay;

import android.annotation.SuppressLint;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.view.MotionEvent;

import java.io.IOException;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.Socket;
import java.net.SocketAddress;
import java.nio.ByteBuffer;

public class TouchMessenger extends Thread {
    public static final int TYPE_INJECT_KEYCODE = 0;
    public static final int TYPE_INJECT_TEXT = 1;
    public static final int TYPE_INJECT_TOUCH_EVENT = 2;
    public static final int TYPE_INJECT_SCROLL_EVENT = 3;
    public static final int TYPE_BACK_OR_SCREEN_ON = 4;
    public static final int TYPE_EXPAND_NOTIFICATION_PANEL = 5;
    public static final int TYPE_COLLAPSE_NOTIFICATION_PANEL = 6;
    public static final int TYPE_GET_CLIPBOARD = 7;
    public static final int TYPE_SET_CLIPBOARD = 8;
    public static final int TYPE_SET_SCREEN_POWER_MODE = 9;
    public static final int TYPE_ROTATE_DEVICE = 10;
    private static final int RAW_BUFFER_SIZE = 4096;


    Handler mHandler;
    private final byte[] rawBuffer = new byte[200];
    private final ByteBuffer msgBuffer = ByteBuffer.wrap(rawBuffer);
    Socket controlSocket;
    OutputStream outputStream;
    InetAddress address;

    public TouchMessenger(){


    }



    @SuppressLint("HandlerLeak")
    @Override
    public void run() {

        try {
            controlSocket = new Socket(address, 7777);
            outputStream = controlSocket.getOutputStream();


        } catch (IOException e1) {
            e1.printStackTrace();
        }
        Looper.prepare();



        mHandler = new Handler() {
            @Override
            public void handleMessage(Message msg) {
                System.out.println("处理消息");
                switch (msg.what) {
                    case TYPE_INJECT_TOUCH_EVENT:
                        int caonima = TYPE_INJECT_TOUCH_EVENT;


                        MotionEvent event = (MotionEvent) msg.obj;



                        byte DataBuffer[] = new byte[12];

                        /*preserved for future touch functions*/
                        byte[] Masked = new byte[4];
                        byte[] ByteofPostionX = new byte[4];
                        byte[] ByteofPostionY = new byte[4];

                         Masked = intToByte(event.getActionMasked());
                         ByteofPostionX = floatToByte(event.getX(event.getPointerCount()-1));
                         ByteofPostionY = floatToByte(event.getY(event.getPointerCount()-1));


                        System.arraycopy(Masked,0,DataBuffer,0,4);
                        System.arraycopy(ByteofPostionX,0,DataBuffer,4,4);
                        System.arraycopy(ByteofPostionY,0,DataBuffer,8,4);

                        System.out.println(ByteofPostionX);
                        System.out.println(ByteofPostionY);

                        try {

                            outputStream.write(DataBuffer);


                            outputStream.flush();
                        } catch (IOException e) {
                            e.printStackTrace();
                        }




                        break;
                    default:
                        break;

                }

              ;
                byte[] output = msgBuffer.array();

                byte [] testX = new byte[4];
                byte [] testy = new byte[4];
                System.arraycopy(output,12,testX,0,4);
                System.arraycopy(output,16,testy,0,4);
                ByteBuffer fuck= ByteBuffer.allocate(8);

                fuck.put(testX);
                float  testXx =fuck.getFloat();
                fuck.clear();
                fuck.put(testy);
                float  testYy =fuck.getFloat();



                super.handleMessage(msg);
            }


        };
        Looper.loop();
    }


    public void getSocketForMessenger(InetAddress address){
        this.address = address;
    }


    public static byte[] floatToByte(float f) {

        // 把float转换为byte[]
        int fbit = Float.floatToIntBits(f);

        byte[] b = new byte[4];
        for (int i = 0; i < 4; i++) {
            b[i] = (byte) (fbit >> (24 - i * 8));
        }

        // 翻转数组
        int len = b.length;
        // 建立一个与源数组元素类型相同的数组
        byte[] dest = new byte[len];
        // 为了防止修改源数组，将源数组拷贝一份副本
        System.arraycopy(b, 0, dest, 0, len);
        byte temp;
        // 将顺位第i个与倒数第i个交换
        for (int i = 0; i < len / 2; ++i) {
            temp = dest[i];
            dest[i] = dest[len - i - 1];
            dest[len - i - 1] = temp;
        }

        return dest;

    }

    public static byte[] intToByte(int num) {
        byte[]bytes=new byte[4];
        bytes[0]=(byte) ((num>>24)&0xff);
        bytes[1]=(byte) ((num>>16)&0xff);
        bytes[2]=(byte) ((num>>8)&0xff);
        bytes[3]=(byte) (num&0xff);
        return bytes;


    }





}
