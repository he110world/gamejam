#ifndef graphics_h__
#define graphics_h__

#include "platform.h"

#if defined(GJ_OS_WINDOWS)
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#elif defined(GJ_OS_MAC)
#include <SDL2/SDL_opengl.h>
#else
#include <SDL2/SDL_opengles2.h>
#endif

#if defined(GJ_OS_WINDOWS)
extern PFNGLBINDVERTEXARRAYOESPROC glBindVertexArrayOES;
extern PFNGLGENVERTEXARRAYSOESPROC glGenVertexArraysOES;
extern PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArraysOES;
#endif

#ifdef GJ_OS_MAC
    #ifndef GL_LUMINANCE
        #define GL_LUMINANCE GL_RED
    #endif

    #ifndef glClearDepthf
        #define glClearDepthf glClearDepth
    #endif
#endif

int initGraphics( int w, int h, int fullscreen );
void printShaderError( const char* name, unsigned int s );
void swapBuffers();

#endif // graphics_h__
