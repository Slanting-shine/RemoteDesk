#ifndef FUCK_MEDIODECODE_OPENGLRENDER_H
#define FUCK_MEDIODECODE_OPENGLRENDER_H


#include <GLES2/gl2.h>
#include <thread>


class OpenGLRender{
public:


    static int loadShader(GLenum shaderType, const char *pSource);
    int CreateOpenGLProgram(const char *pVertexShaderSource, const char *pFragmentShaderSource);

    void OnSurfaceCreated();
    void OnSurfaceChanged(int w, int h);
    void OnDrawFrame();
    int getTexture();

    static OpenGLRender *GetInstance();
    static void ReleaseInstance();



private:
    OpenGLRender();
    virtual ~OpenGLRender();

    static std::mutex m_Mutex;
    static OpenGLRender* s_Instance;
    GLuint m_ProgramObj = GL_NONE;
    GLuint m_TextureId;
    GLuint glHPosition;
    GLuint glHCoordinate;
    GLuint glHTexture;


};


#endif //FUCK_MEDIODECODE_OPENGLRENDER_H
