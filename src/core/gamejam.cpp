#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_keyboard.h>
#include <sys/types.h>
#include <sys/stat.h>
//#include <unistd.h>
#include "FileWatcher.h"
#include <map>
#include <string>
#include <math.h>
#include <curl/curl.h>
#include "graphics.h"
#include "imgui.h"
//#ifdef GL_OS_OSX
//#include "imguiRenderGL3.h"
//#else
#include "imguiRenderGLES2.h"
//#endif
#include "render.h"
#include <libtcc.h>

/* screen width, height, and bit depth */
#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP     32

int xres = SCREEN_WIDTH;
int yres = SCREEN_HEIGHT;

/* Define our booleans */
#define TRUE  1
#define FALSE 0

int keystat[256];
int mousestat[10];
int mousex, mousey, mousexr, mouseyr;
int recompile();
int scratchpad[10000];

SDL_Window* displayWindow;
SDL_Renderer* displayRenderer;

// file watcher
class RecompileListener : public FW::FileWatchListener
{
public:
	RecompileListener() {}

	void handleFileAction( FW::WatchID watchid, const FW::String& dir, const FW::String& filename, FW::Action action ) {
		switch( action )
		{
			//		case FW::Actions::Add:
			//			printf("add %s\n", filename.c_str());
			//			break;
		case FW::Actions::Modified:
            if( filename.find("update.c")!=std::string::npos )
			{
				recompile();		
			}
			break;
			//		case FW::Actions::Delete:
			//			printf("del %s\n", filename.c_str());
			//			break;
        default:
                break;
		}
	}
};

/* This is our SDL surface */
SDL_Surface *surface;

/* function to release/destroy our resources and restoring the old desktop */
void Quit( int returnCode )
{
	/* clean up the window */
	SDL_Quit( );

	/* and exit appropriately */
	exit( returnCode );
}

#if 0
static void gluPerspective( GLdouble fovY, GLdouble aspect, GLdouble zNear, GLdouble zFar )
{
    const GLdouble pi = 3.1415926535897932384626433832795;
    GLdouble fW, fH;

    //fH = tan( (fovY / 2) / 180 * pi ) * zNear;
    fH = tan( fovY / 360 * pi ) * zNear;
    fW = fH * aspect;

    glFrustum( -fW, fW, -fH, fH, zNear, zFar );
}
#endif

#if 0
/* function to reset our viewport after a window resize */
int resizeWindow( int width, int height )
{
	xres = width;
	yres = height;

	/* Height / width ration */
	GLfloat ratio;

	/* Protect against a divide by zero */
	if ( height == 0 )
		height = 1;

	ratio = ( GLfloat )width / ( GLfloat )height;

	/* Setup our viewport. */
	glViewport( 0, 0, ( GLsizei )width, ( GLsizei )height );

	/* change to the projection matrix and set our viewing volume. */
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );

	/* Set our perspective */
	gluPerspective( 45.0f, ratio, 0.1f, 100.0f );

	/* Make sure we're chaning the model view and not the projection */
	glMatrixMode( GL_MODELVIEW );

	/* Reset The View */
	glLoadIdentity( );

	return( TRUE );
}
#endif

/* function to handle key press events */
void handleKeyPress( SDL_Keysym *keysym )
{
	switch ( keysym->sym )
	{
	case SDLK_ESCAPE:
		/* ESC key was pressed */
		Quit( 0 );
		break;
	case SDLK_F1:
		/* F1 key was pressed
		* this toggles fullscreen mode
		*/
		//SDL_WM_ToggleFullScreen( surface );
		break;
	default:
		keystat[keysym->sym%256] = 1;
		break;
	}

	return;
}

void initInput()
{
	memset( keystat, 0, sizeof(keystat) );
	memset( mousestat, 0, sizeof(mousestat) );
	mousex = mousey = 0;
}

/* general OpenGL initialization function */
int initGL()
{

	/* Enable smooth shading */
//	glShadeModel( GL_SMOOTH );

	/* Set the background black */
	glClearColor( 1.0f, 0.0f, 0.0f, 0.0f );

	/* Depth buffer setup */
	glClearDepthf( 1.0f );

	/* Enables Depth Testing */
	glEnable( GL_DEPTH_TEST );

	/* The Type Of Depth Test To Do */
	glDepthFunc( GL_LEQUAL );

    glViewport( 0, 0, ( GLsizei )800, ( GLsizei )600 );

	/* Really Nice Perspective Calculations */
//	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

	return( TRUE );
}

typedef void (*updatefp_t)();
//void (*drawGLScene)() = 0;
updatefp_t update = 0;
TCCState* g_tcc = 0;

static void compile_err(void *d, const char *s)
{
	printf("error n%ld (%s)", (long)d, s);
}

void clearColor( float r, float g, float b, float a )
{
	glClearColor(r,g,b,a);
	glClear ( GL_COLOR_BUFFER_BIT );
}

void addSymbols(TCCState* s)
{
	tcc_add_symbol(s, "keystat", &keystat);
	tcc_add_symbol(s, "mousestat", &mousestat);
	tcc_add_symbol(s, "mousex", &mousex);
	tcc_add_symbol(s, "mousey", &mousey);
	tcc_add_symbol(s, "mousexr", &mousexr);
	tcc_add_symbol(s, "mouseyr", &mouseyr);
	tcc_add_symbol(s, "xres", &xres);
	tcc_add_symbol(s, "yres", &yres);
	tcc_add_symbol(s, "scratchpad", &scratchpad);

	tcc_add_symbol(s, "clearColor", (void*)clearColor);

	// IMGUI
	tcc_add_symbol(s, "guiBeginFrame", (void*)imguiBeginFrame);
	tcc_add_symbol(s, "guiEndFrame", (void*)imguiEndFrame);
	tcc_add_symbol(s, "guiBeginScrollArea", (void*)imguiBeginScrollArea);
	tcc_add_symbol(s, "guiEndScrollArea", (void*)imguiEndScrollArea);

	tcc_add_symbol(s, "guiIndent", (void*)imguiIndent);
	tcc_add_symbol(s, "guiUnindent", (void*)imguiUnindent);
	tcc_add_symbol(s, "guiSeparator", (void*)imguiSeparator);
	tcc_add_symbol(s, "guiSeparatorLine", (void*)imguiSeparatorLine);

	tcc_add_symbol(s, "guiButton", (void*)imguiButton);
	tcc_add_symbol(s, "guiItem", (void*)imguiItem);
	tcc_add_symbol(s, "guiCheck", (void*)imguiCheck);
	tcc_add_symbol(s, "guiCollapse", (void*)imguiCollapse);
	tcc_add_symbol(s, "guiLabel", (void*)imguiLabel);
	tcc_add_symbol(s, "guiValue", (void*)imguiValue);
	tcc_add_symbol(s, "guiSlider", (void*)imguiSlider);

	tcc_add_symbol(s, "guiDrawText", (void*)imguiDrawText);
	tcc_add_symbol(s, "guiDrawLine", (void*)imguiDrawLine);
	tcc_add_symbol(s, "guiDrawRoundedRect", (void*)imguiDrawRoundedRect);
	tcc_add_symbol(s, "guiDrawRect", (void*)imguiDrawRect);
}

/*time_t lastmodtime;*/
int recompile()
{
	/*
	struct stat file_stat;
	int err = stat("update.c", &file_stat);

	if( !g_tcc )
	{
	lastmodtime = file_stat.st_mtime;		
	}
	else
	{
	if( lastmodtime == file_stat.st_mtime )
	return 0;
	lastmodtime = file_stat.st_mtime;
	}
	*/
	printf("recompile\n");
	FILE* f = fopen( "update.c", "rb" );	// MUST be "rb" on windows - othersize CRLF will be converted to \n, and file size will be wrong (CRLF:2bytes, \n:1bytes)
	fseek(f, 0L, SEEK_END);
	long sz = ftell(f);
	fseek(f, 0L, SEEK_SET);
	char* program = (char*)malloc( sz+1 );
	fread( program, sz, 1, f ); 
	program[sz] = '\0';
	fclose(f);

	TCCState* s = tcc_new();
	tcc_set_output_type(s, TCC_OUTPUT_MEMORY);

	int n;
	tcc_set_error_func(s, &n, compile_err);

	addSymbols(s);

	int compileResult = tcc_compile_string(s, program);
	free( program );

	if ( compileResult == -1)
	{
		return 1;
	}
	
	tcc_add_library_path(s,".");
	
	/* relocate the code */
	if (tcc_relocate(s, TCC_RELOCATE_AUTO) < 0)
		return 1;

	/* get entry symbol */
	update = (updatefp_t)tcc_get_symbol(s, "update");
	if (!update)
		return 1;

	if( g_tcc ) tcc_delete(g_tcc);
	g_tcc = s;

	printf("recompile ok\n");
	return 0;
}

int main(int argc, char **argv)
{
//	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
//	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);

	/* initialize OpenGL */
	initGraphics(800,600,0);
	initGL( );
	initInput();
	renderInit();
	imguiRenderGLInit("DroidSans.ttf");
	curl_global_init(CURL_GLOBAL_ALL); 

	FW::FileWatcher* watcher = new FW::FileWatcher();
	/*FW::WatchID watchid = */watcher->addWatch( "./", new RecompileListener() );

	recompile();
	/* resize the initial window */
//	resizeWindow( xres, yres );

	/* These are to calculate our fps */
	static GLint T0     = 0;
	static GLint Frames = 0;



	/* wait for events */
	bool done = false;
	SDL_Event event;
	while ( !done )
	{
		/* handle the events in the queue */

		while ( SDL_PollEvent( &event ) )
		{
			switch( event.type )
			{
			//case SDL_ACTIVEEVENT:
				/* Something's happend with our focus
				* If we lost focus or we are iconified, we
				* shouldn't draw the screen
				*/
				//if ( event.active.gain == 0 )
				//	isActive = FALSE;
				//   else
				//		isActive = TRUE;
			//	break;			    
#if 0
			case SDL_VIDEORESIZE:
				/* handle resize event */
				surface = SDL_SetVideoMode( event.resize.w,
					event.resize.h,
					16, videoFlags );
				if ( !surface )
				{
					fprintf( stderr, "Could not get a surface after resize: %s\n", SDL_GetError( ) );
					Quit( 1 );
				}
				resizeWindow( event.resize.w, event.resize.h );
				break;
#endif
			case SDL_KEYDOWN:
				/* handle key presses */
				handleKeyPress( &event.key.keysym );
				break;
			case SDL_KEYUP:
                    if(event.key.keysym.sym<=255){
                        keystat[event.key.keysym.sym] = 0;
                    }
                    break;
			case SDL_MOUSEMOTION:
				mousexr = event.motion.xrel;
				mouseyr = event.motion.yrel;
				mousex = event.motion.x;
				mousey = event.motion.y;
				break;
			case SDL_MOUSEBUTTONDOWN:
				mousestat[event.button.button] = 1;
				break;
			case SDL_MOUSEBUTTONUP:
				mousestat[event.button.button] = 0;
				break;
			case SDL_QUIT:
				/* handle quit requests */
				done = TRUE;
				break;
			default:
				break;
			}
		}

		/* draw the scene */
		//if ( isActive )
		{
			watcher->update();
            
            //glClearColor(1, 0, 0, 1);
            //glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
            
			if(update)
			{
				update();
			}
            /*
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            float projection[16] = { 2.f/800, 0.f, 0.f,  0.f,
                0.f, 2.f/600,  0.f,  0.f,
                0.f,  0.f, -2.f, 0.f,
                -1.f, -1.f,  -1.f,  1.f };
            glLoadMatrixf(projection);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();*/
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDisable(GL_DEPTH_TEST);
			imguiRenderGLDraw(800, 600);
            
/*
            glBegin(GL_TRIANGLES);
            glColor3f(1, 1, 0);
            glVertex2d(1, 1);
            glVertex2d(1, 0);
            glVertex2d(0, 1);
            glEnd();
*/
			/* Draw it to the screen */
			swapBuffers();
			
			/* Gather our frames per second */
			Frames++;
			{
				GLint t = SDL_GetTicks();
				if (t - T0 < 16) {
//					SDL_Delay(16 - (t-T0));

//					GLfloat seconds = (t - T0) / 1000.f;
//					GLfloat fps = Frames / seconds;
//					Frames = 0;
				}

				T0 = t;
			}

		}
	}

	imguiRenderGLDestroy();
	renderFini();

	/* clean ourselves up and exit */
	Quit( 0 );

	/* Should never get here */
	return( 0 );
}
