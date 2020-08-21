package com.noob.remotedisplay.JNIBaseOpenGL;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;

import com.noob.remotedisplay.VideoDecodeThread;

import java.net.InetAddress;


public class BaseGLView extends GLSurfaceView implements SurfaceHolder.Callback{

    BaseGLRender mBaseGLRender;
    private VideoDecodeThread thread;

    public BaseGLView(Context context){
        super(context);
        setEGLContextClientVersion(2);
        mBaseGLRender = new BaseGLRender();
        setRenderer(mBaseGLRender);
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
        mBaseGLRender.setOnRenderListener(new BaseGLRender.OnRenderListener() {
            @Override
            public void onRender() {
                requestRender();
            }
        });
    }

    public BaseGLView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public void getSocketForDecoder(InetAddress address){

        thread = new VideoDecodeThread(mBaseGLRender.surface,address);
    }

    public void start(){
        Log.e("VideoPlayView", "start");

        thread.start();
    }

    public void stop(){
        thread.interrupt();

    }


}
