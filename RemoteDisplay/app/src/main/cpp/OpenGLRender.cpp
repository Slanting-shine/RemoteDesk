
#include "OpenGLRender.h"
#include "LogUtils.h"
#include <stdlib.h>
#include <GLES2/gl2ext.h>
#include <iostream>


OpenGLRender* OpenGLRender::s_Instance = nullptr;
std::mutex OpenGLRender::m_Mutex;

//顶点着色器
static char vertexShaderStr[] =
        "attribute vec4 vPosition;\n"
        "attribute vec2 vCoordinate;\n"
        "varying vec2 aCoordinate;\n"
        "void main(){\n"
        "    gl_Position=vPosition;\n"
        "    aCoordinate=vCoordinate;\n"
        "}\n";

//片状着色器
static char fragmentShaderStr[] =
        "#extension GL_OES_EGL_image_external : require\n"
        "precision mediump float;\n"
        "uniform  samplerExternalOES vTexture;\n"
        "varying vec2 aCoordinate;\n"
        "void main(){\n"
        "    gl_FragColor=texture2D(vTexture,aCoordinate);\n"
        "}\n";

//顶点坐标
GLfloat verticesCoords[] = {
        -1.0f,1.0f,    //左上角 V1
        -1.0f,-1.0f,   //左下角 V2
        1.0f,1.0f,    //右上角 V3
        1.0f,-1.0f    //右下角 V4
};

//纹理坐标
GLfloat textureCoords[] = {
        0.0f,0.0f,  //左上角 V1
        0.0f,1.0f,  //左下角 V2
        1.0f,0.0f,   //右上角 V3
        1.0f,1.0f  //右下角 V4
};



OpenGLRender::OpenGLRender() {

}

OpenGLRender::~OpenGLRender() {

    OpenGLRender::ReleaseInstance();
}





int OpenGLRender::loadShader(GLenum shaderType, const char *pSource) {
    int shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &pSource, NULL);
    glCompileShader(shader);
    return shader;
}


/**
 * 成OpenGL Program
 *
 * @param vertexSource      顶点着色器代码
 * @param fragmentSource    片元着色器代码
 * @return                   生成的OpenGL Program，如果为0，则表示创建失败
 */
int OpenGLRender::CreateOpenGLProgram(const char *pVertexShaderSource, const char *pFragmentShaderSource){

    int vertex = loadShader(GL_VERTEX_SHADER,pVertexShaderSource);
    int fragment = loadShader(GL_FRAGMENT_SHADER,pFragmentShaderSource);
    int program = glCreateProgram();

    if (program != 0) {
        glAttachShader(program, vertex);
        glAttachShader(program, fragment);
        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

        if (linkStatus != GL_TRUE) {
           // Log.e(TAG, "Could not link program:" + GLES30.glGetProgramInfoLog(program));
            glDeleteProgram(program);
            program = 0;
        }
    }
    return program;

}

void OpenGLRender::OnSurfaceCreated() {

    m_ProgramObj = CreateOpenGLProgram(vertexShaderStr,fragmentShaderStr);

    glGenTextures(1, &m_TextureId);

    glHPosition = glGetAttribLocation(m_ProgramObj,"vPosition");
    glHCoordinate = glGetAttribLocation(m_ProgramObj,"vCoordinate");
    glHTexture = glGetUniformLocation(m_ProgramObj,"u_texture");

    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, m_TextureId);

}

void OpenGLRender::OnSurfaceChanged(int width, int height) {
    //参数X，Y指定了视见区域的左下角在窗口中的位置，一般情况下为（0，0）(屏幕左上角)，Width和Height指定了视见区域的宽度和高度。
    glViewport(0,0,width,height);


}

void OpenGLRender::OnDrawFrame() {
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glUseProgram(m_ProgramObj);

    glEnableVertexAttribArray(glHPosition);
    glVertexAttribPointer(glHPosition,2,GL_FLOAT,GL_FALSE,8,verticesCoords);
    glEnableVertexAttribArray(glHCoordinate);
    glVertexAttribPointer(glHCoordinate,2,GL_FLOAT,GL_FALSE,8,textureCoords);

    glActiveTexture(GL_TEXTURE0);

    glUniform1i(glHTexture, 0);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);


}

OpenGLRender *OpenGLRender::GetInstance() {
    if(s_Instance == nullptr)
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        if(s_Instance == nullptr){
            s_Instance = new OpenGLRender();
        }
    }
    return s_Instance;
}

void OpenGLRender::ReleaseInstance() {
    if(s_Instance != nullptr)
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        if(s_Instance != nullptr){
            delete s_Instance;
            s_Instance = nullptr;
        }
    }
}

int OpenGLRender::getTexture() {
    if(m_TextureId != NULL){
        return m_TextureId;
    }
    return NULL;
}


