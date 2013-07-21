#ifndef graphics_h__
#define graphics_h__

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

extern PFNGLBINDVERTEXARRAYOESPROC glBindVertexArrayOES;
extern PFNGLGENVERTEXARRAYSOESPROC glGenVertexArraysOES;
extern PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArraysOES;

int initGraphics( int w, int h, int fullscreen );
void printShaderError( const char* name, GLuint s );
void swapBuffers();

#endif // graphics_h__
