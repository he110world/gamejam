#include "graphics.h"
#include "platform.h"

#if defined(GJ_OS_WINDOWS)
#include <EGL/egl.h>
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <stdio.h>


#ifdef GJ_OS_WINDOWS
typedef struct 
{ 
	/// Window width 
	GLint       width; 
	/// Window height 
	GLint       height; 
	/// Window handle 
	EGLNativeWindowType  hWnd; 
	/// EGL display 
	EGLDisplay  eglDisplay; 
	/// EGL context 
	EGLContext  eglContext; 
	/// EGL surface 
	EGLSurface  eglSurface; 
} ESContext; 

static EGLBoolean CreateEGLContext ( EGLNativeWindowType hWnd, EGLDisplay* eglDisplay, 
									EGLContext* eglContext, EGLSurface* eglSurface, 
									EGLint* configAttribList, EGLint* surfaceAttribList) 
{ 
	EGLint numConfigs; 
	EGLint majorVersion; 
	EGLint minorVersion; 
	EGLDisplay display; 
	EGLContext context; 
	EGLSurface surface; 
	EGLConfig config; 
	EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE, EGL_NONE }; 

	// Get Display 
	display = eglGetDisplay(GetDC(hWnd)); // EGL_DEFAULT_DISPLAY 
	if ( display == EGL_NO_DISPLAY ) 
	{ 
		return EGL_FALSE; 
	} 

	// Initialize EGL 
	LoadLibrary("d3dcompiler_43.dll");	// HACK: MUST call this, otherwise ANGEL can't find d3d dlls
	if ( !eglInitialize(display, &majorVersion, &minorVersion) ) 
	{ 
		return EGL_FALSE; 
	} 

	// Get configs 
	if ( !eglGetConfigs(display, NULL, 0, &numConfigs) ) 
	{ 
		return EGL_FALSE; 
	} 

	// Choose config 
	if ( !eglChooseConfig(display, configAttribList, &config, 1, &numConfigs) ) 
	{ 
		return EGL_FALSE; 
	} 

	// Create a surface 
	surface = eglCreateWindowSurface(display, config, (EGLNativeWindowType)hWnd, surfaceAttribList); 
	if ( surface == EGL_NO_SURFACE ) 
	{ 
		return EGL_FALSE; 
	} 

	// Create a GL context 
	context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs ); 
	if ( context == EGL_NO_CONTEXT ) 
	{ 
		return EGL_FALSE; 
	}    

	// Make the context current 
	if ( !eglMakeCurrent(display, surface, surface, context) ) 
	{ 
		return EGL_FALSE; 
	} 

	*eglDisplay = display; 
	*eglSurface = surface; 
	*eglContext = context; 
	return EGL_TRUE; 
} 
#endif

SDL_Window *g_window = NULL;

#ifdef GJ_OS_WINDOWS
ESContext* g_context = NULL;
PFNGLBINDVERTEXARRAYOESPROC glBindVertexArrayOES = NULL;
PFNGLGENVERTEXARRAYSOESPROC glGenVertexArraysOES = NULL;
PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArraysOES = NULL;
#else
SDL_GLContext g_context = NULL;
#endif


static void initGLESExtensions()
{
#if 0
	// TODO: ANGEL doesn't support these extensions
	glBindVertexArrayOES = (PFNGLBINDVERTEXARRAYOESPROC)eglGetProcAddress("glBindVertexArrayOES");
	glGenVertexArraysOES = (PFNGLGENVERTEXARRAYSOESPROC)eglGetProcAddress("glGenVertexArrayOES");
	glDeleteVertexArraysOES = (PFNGLDELETEVERTEXARRAYSOESPROC)eglGetProcAddress("glDeleteVertexArrayOES");
#endif
}

int initGraphics( int w, int h, int fullscreen )
{
	int result = SDL_Init( /*SDL_INIT_TIMER |*/ SDL_INIT_AUDIO | SDL_INIT_VIDEO );

#ifdef GJ_OS_WINDOWS
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
#endif

#ifdef GL_OS_OSX
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif
    
	g_window = SDL_CreateWindow(NULL, 100, 100, w, h, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN/* | SDL_WINDOW_BORDERLESS*/ | (fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0));

#ifdef GJ_OS_WINDOWS
	SDL_SysWMinfo info; 
	SDL_VERSION(&info.version); // initialize info structure with SDL version info 
	SDL_bool get_win_info = SDL_GetWindowWMInfo(g_window, &info); 
	SDL_assert_release(get_win_info); 
	EGLNativeWindowType hWnd = info.info.win.window; 

	g_context = new ESContext(); 

	g_context->width = w;
	g_context->height = h;
	g_context->hWnd = hWnd; 

	EGLint configAttribList[] = 
	{ 
		EGL_RED_SIZE,       8, 
		EGL_GREEN_SIZE,     8, 
		EGL_BLUE_SIZE,      8, 
		EGL_ALPHA_SIZE,     8 /*: EGL_DONT_CARE*/, 
		EGL_DEPTH_SIZE,     EGL_DONT_CARE, 
		EGL_STENCIL_SIZE,   EGL_DONT_CARE, 
		EGL_SAMPLE_BUFFERS, 0, 
		EGL_NONE 
	}; 
	EGLint surfaceAttribList[] = 
	{ 
		// attribute, value
		EGL_NONE, EGL_NONE 
	}; 

	if ( !g_context ) 
	{ 
		//throw std::runtime_error("can't create opengl es 2.0 context, NULL es_context"); 
	} 

	EGLBoolean is_context_created = CreateEGLContext(g_context->hWnd, 
		&g_context->eglDisplay, 
		&g_context->eglContext, 
		&g_context->eglSurface, 
		configAttribList, 
		surfaceAttribList); 

	if (is_context_created == 0) 
	{ 
		//throw std::runtime_error("can't create opengl es 2.0 context"); 
	} 
#else 
	g_context = SDL_GL_CreateContext(g_window); 
	//int result = SDL_GL_SetSwapInterval(0); 
	//SDL_assert(0 == result); 
#endif 

	initGLESExtensions();

	return result;
}

void swapBuffers()
{
#ifdef GJ_OS_WINDOWS
	eglSwapBuffers( g_context->eglDisplay, g_context->eglSurface ); 
#else
	SDL_GL_SwapWindow(g_window);
#endif
}

void printShaderError( const char* name, unsigned int s )
{
	int status;
	glGetShaderiv(s, GL_COMPILE_STATUS, &status);
    if(status != GL_TRUE){
        char errorlog[1024] = {'\0'};
        glGetShaderInfoLog(s, 1024, 0, errorlog);
		printf("%s error:\n%s", name, errorlog);
		printf("--------------------------------------------------\n");
    }

}
