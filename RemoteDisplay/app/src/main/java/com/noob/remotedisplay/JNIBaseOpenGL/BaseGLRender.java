package com.noob.remotedisplay.JNIBaseOpenGL;

import android.graphics.SurfaceTexture;
import android.opengl.GLSurfaceView;
import android.view.Surface;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class BaseGLRender implements GLSurfaceView.Renderer , SurfaceTexture.OnFrameAvailableListener {
    static {
        System.loadLibrary("OpenGL-JNI");
    }

    //for video openGL render
    public static native void native_OnSurfaceCreated();
    public static native void native_OnSurfaceChanged(int width, int height);
    public static native void native_OnDrawFrame();
    public static native int native_getTexture();

    private SurfaceTexture surfaceTexture;
    public Surface surface;
    private OnRenderListener onRenderListener;
    private OnSurfaceCreateListener onSurfaceCreateListener;


    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {

        native_OnSurfaceCreated();
        System.out.println("create");

        surfaceTexture = new SurfaceTexture(native_getTexture());
        System.out.println(native_getTexture());
        surface = new Surface(surfaceTexture);

        surfaceTexture.setOnFrameAvailableListener(this);

        if (onSurfaceCreateListener != null) {
            //将Surface回掉出去给MediaCodec绑定渲染
            onSurfaceCreateListener.onSurfaceCreate(surface);
        }

    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        native_OnSurfaceChanged(width, height);
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        surfaceTexture.updateTexImage();
        native_OnDrawFrame();
        System.out.println("drawFrame");
    }


    public void setOnSurfaceCreateListener(OnSurfaceCreateListener onSurfaceCreateListener) {
        this.onSurfaceCreateListener = onSurfaceCreateListener;
    }

    public void setOnRenderListener(OnRenderListener onRenderListener) {
        this.onRenderListener = onRenderListener;
    }

    @Override
    public void onFrameAvailable(SurfaceTexture surfaceTexture) {

        if (onRenderListener != null) {
            ////将onFrameAvailable函数回掉到GLSurfaceView调用requestRender()触发onDrawFrame()
            onRenderListener.onRender();
        }
    }


    public interface OnRenderListener {
        void onRender();
    }



    public interface OnSurfaceCreateListener {
        void onSurfaceCreate(Surface surface);
    }

}
