
#include <jni.h>
#include "OpenGLRender.h"

extern "C" JNIEXPORT void JNICALL
Java_com_noob_remotedisplay_JNIBaseOpenGL_BaseGLRender_native_1OnSurfaceCreated(JNIEnv *env,
                                                                           jclass clazz) {
    OpenGLRender::GetInstance()->OnSurfaceCreated();
}

extern "C" JNIEXPORT void JNICALL
Java_com_noob_remotedisplay_JNIBaseOpenGL_BaseGLRender_native_1OnSurfaceChanged(JNIEnv *env,
                                                                           jclass clazz, jint width,
                                                                           jint height) {
    OpenGLRender::GetInstance()->OnSurfaceChanged(width, height);
}

extern "C" JNIEXPORT void JNICALL
Java_com_noob_remotedisplay_JNIBaseOpenGL_BaseGLRender_native_1OnDrawFrame(JNIEnv *env, jclass clazz) {
    OpenGLRender::GetInstance()->OnDrawFrame();
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_noob_remotedisplay_JNIBaseOpenGL_BaseGLRender_native_1getTexture(JNIEnv *env,
                                                                          jclass clazz) {
    return OpenGLRender::GetInstance()->getTexture();

}