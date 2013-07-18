/*
* This code was created by Jeff Molofee '99 
* (ported to Linux/SDL by Ti Leggett '01)
*
* If you've found this code useful, please let me know.
*
* Visit Jeff at http://nehe.gamedev.net/
* 
* or for port-specific comments, questions, bugreports etc. 
* email to leggett@eecs.tulane.edu
*/

#include <stdio.h>
#include <stdlib.h>
//#include <GL/gl.h>
//#include <GL/glu.h>
#include "SDL.h"
#include "SDL_opengl.h"
#include "SDL_keyboard.h"
#include <libtcc.h>
#include <sys/types.h>
#include <sys/stat.h>
//#include <unistd.h>
#include "FileWatcher.h"
#include <map>
#include <string>
#include "stb_image.h"
//#include "http_lib.h"
#include <pthread.h>
#include <curl/curl.h>
#include <math.h>

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
			if( filename == "draw.c" )
			{
				recompile();		
			}
			break;
			//		case FW::Actions::Delete:
			//			printf("del %s\n", filename.c_str());
			//			break;
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

static void gluPerspective( GLdouble fovY, GLdouble aspect, GLdouble zNear, GLdouble zFar )
{
    const GLdouble pi = 3.1415926535897932384626433832795;
    GLdouble fW, fH;

    //fH = tan( (fovY / 2) / 180 * pi ) * zNear;
    fH = tan( fovY / 360 * pi ) * zNear;
    fW = fH * aspect;

    glFrustum( -fW, fW, -fH, fH, zNear, zFar );
}

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
	glShadeModel( GL_SMOOTH );

	/* Set the background black */
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

	/* Depth buffer setup */
	glClearDepth( 1.0f );

	/* Enables Depth Testing */
	glEnable( GL_DEPTH_TEST );

	/* The Type Of Depth Test To Do */
	glDepthFunc( GL_LEQUAL );

	/* Really Nice Perspective Calculations */
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

	return( TRUE );
}

pthread_mutex_t s_textureMutex = PTHREAD_MUTEX_INITIALIZER;
struct Texture
{
	Texture() : img(NULL) {}
	Texture( unsigned texid ) : id(texid), img(NULL) {}

	unsigned	id;
	int		x,y,comp;
	unsigned char*	img;	//init=0; if loaded=>!0
};

static std::map<std::string, Texture>	s_textures;

struct MemoryStruct {
	char *memory;
	size_t size;
};

static size_t
curlDownloadImage_cb(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)userp;

	mem->memory = (char*)realloc(mem->memory, mem->size + realsize + 1);
	if (mem->memory == NULL) {
		/* out of memory! */ 
		printf("not enough memory (realloc returned NULL)\n");
		exit(EXIT_FAILURE);
	}

	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	return realsize;
}

static void *loadTexture_cb(void *url)
{
	char* texname = (char*)url;
	std::map<std::string, Texture>::iterator it = s_textures.find(texname);
	if( it == s_textures.end() ) return NULL;

	Texture& tex = it->second;

	// load from disk	
	if( !strstr( texname, "http" ) )
	{
		tex.img = stbi_load( texname, &tex.x, &tex.y, &tex.comp, 0 ); 	
		if( !tex.img )
		{
			printf( "texture %s not found!\n", texname );
			return NULL;
		}

	}
	// load from web
	else
	{
		CURL *curl;                                          

		curl = curl_easy_init();                             
		curl_easy_setopt(curl, CURLOPT_URL, url);            

		/* send all data to this function  */ 
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlDownloadImage_cb);

		/* we pass our 'chunk' struct to the callback function */ 
		struct MemoryStruct chunk;
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);

		/* some servers don't like requests that are made without a user-agent
		field, so we provide one */ 
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
		curl_easy_perform(curl); /* ignores error */         
		curl_easy_cleanup(curl);                             

		if( chunk.memory )
		{
			tex.img = stbi_load_from_memory( (stbi_uc*)chunk.memory, chunk.size, &tex.x, &tex.y, &tex.comp, 0 ); 
			free( chunk.memory );
		}
	}

	return NULL;
}

void glTexture( const char* texname )
{
	// disable texture
	if( !texname )
	{
		glBindTexture( GL_TEXTURE_2D, 0 );
		glDisable(GL_TEXTURE_2D);
		return;
	}

	std::map<std::string,Texture>::iterator it = s_textures.find( texname );	
	// found
	if( it != s_textures.end() )
	{
		Texture& tex = it->second;

		glBindTexture( GL_TEXTURE_2D, it->second.id );
		if( tex.img )	// texture loaded but not inited
		{
			// init texture to opengl
			GLenum type = tex.comp==4 ? GL_RGBA : GL_RGB;
			glTexImage2D(GL_TEXTURE_2D, 0, type, tex.x, tex.y, 0, type, GL_UNSIGNED_BYTE,tex.img);	

			free(tex.img);
			tex.img = NULL;

		}
	}
	else	// new texture
	{
		// generate temp texture
		unsigned texid;
		glGenTextures(1, &texid);
		glBindTexture(GL_TEXTURE_2D, texid);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);

		unsigned char dummytex[4] = {255,255,255,255};	

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,1,1,0,GL_RGBA,GL_UNSIGNED_BYTE,dummytex);

		Texture newtex(texid);
		s_textures[texname] = newtex;

		// create a thread to load it
		pthread_t tid;
		pthread_create( &tid, NULL, loadTexture_cb, (void*)texname ); 	
	}

	glEnable( GL_TEXTURE_2D );
}

typedef void (*drawfp_t)();
//void (*drawGLScene)() = 0;
drawfp_t drawGLScene = 0;
TCCState* g_tcc = 0;

void err(void *d, const char *s)
{
	printf("error n%ld (%s)", d, s);
}


/*time_t lastmodtime;*/
int recompile()
{
	/*
	struct stat file_stat;
	int err = stat("draw.c", &file_stat);

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
	FILE* f = fopen( "draw.c", "r" );
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
	tcc_set_error_func(s, &n, err);
	if (tcc_compile_string(s, program) == -1)
		return 1;

	tcc_add_symbol(s, "keystat", keystat);
	tcc_add_symbol(s, "mousestat", mousestat);
	tcc_add_symbol(s, "mousex", &mousex);
	tcc_add_symbol(s, "mousey", &mousey);
	tcc_add_symbol(s, "mousexr", &mousexr);
	tcc_add_symbol(s, "mouseyr", &mouseyr);
	tcc_add_symbol(s, "glTexture", (void*)glTexture);	
	tcc_add_symbol(s, "xres", &xres);
	tcc_add_symbol(s, "yres", &yres);

	free( program );
	/* relocate the code */
	if (tcc_relocate(s, TCC_RELOCATE_AUTO) < 0)
		return 1;

	/* get entry symbol */
	drawGLScene = (drawfp_t)tcc_get_symbol(s, "drawGLScene");
	if (!drawGLScene)
		return 1;

	if( g_tcc ) tcc_delete(g_tcc);
	g_tcc = s;

	printf("recompile ok\n");
	return 0;
}

int main(int argc, char **argv)
{
#if 0
	/* Flags to pass to SDL_SetVideoMode */
	int videoFlags;
	/* main loop variable */
	int done = FALSE;
	/* used to collect events */
	SDL_Event event;
	/* this holds some info about our display */
	const SDL_VideoInfo *videoInfo;
	/* whether or not the window is active */
	int isActive = TRUE;

	/* initialize SDL */
	if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		fprintf( stderr, "Video initialization failed: %s\n",
			SDL_GetError( ) );
		Quit( 1 );
	}

	/* Fetch the video info */
	videoInfo = SDL_GetVideoInfo( );

	if ( !videoInfo )
	{
		fprintf( stderr, "Video query failed: %s\n",
			SDL_GetError( ) );
		Quit( 1 );
	}

	/* the flags to pass to SDL_SetVideoMode */
	videoFlags  = SDL_OPENGL;          /* Enable OpenGL in SDL */
	videoFlags |= SDL_GL_DOUBLEBUFFER; /* Enable double buffering */
	videoFlags |= SDL_HWPALETTE;       /* Store the palette in hardware */
	videoFlags |= SDL_RESIZABLE;       /* Enable window resizing */

	/* This checks to see if surfaces can be stored in memory */
	if ( videoInfo->hw_available )
		videoFlags |= SDL_HWSURFACE;
	else
		videoFlags |= SDL_SWSURFACE;

	/* This checks if hardware blits can be done */
	if ( videoInfo->blit_hw )
		videoFlags |= SDL_HWACCEL;

	/* Sets up OpenGL double buffering */
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

	/* get a SDL surface */
	surface = SDL_SetVideoMode( xres, yres, SCREEN_BPP,
		videoFlags );

	/* Verify there is a surface */
	if ( !surface )
	{
		fprintf( stderr,  "Video mode set failed: %s\n", SDL_GetError( ) );
		Quit( 1 );
	}
#else

    SDL_Init(SDL_INIT_VIDEO);
    SDL_RendererInfo displayRendererInfo;
    SDL_CreateWindowAndRenderer(800, 600, SDL_WINDOW_OPENGL, &displayWindow, &displayRenderer);
    SDL_GetRendererInfo(displayRenderer, &displayRendererInfo);
    /*TODO: Check that we have OpenGL */
    if ((displayRendererInfo.flags & SDL_RENDERER_ACCELERATED) == 0 || 
        (displayRendererInfo.flags & SDL_RENDERER_TARGETTEXTURE) == 0) {
        /*TODO: Handle this. We have no render surface and not accelerated. */
    }
    
#endif



	/* initialize OpenGL */
	initGL( );
	initInput();
	curl_global_init(CURL_GLOBAL_ALL); 

	FW::FileWatcher* watcher = new FW::FileWatcher();
	FW::WatchID watchid = watcher->addWatch( "./", new RecompileListener() );

	recompile();
	/* resize the initial window */
	resizeWindow( xres, yres );

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
				keystat[event.key.keysym.sym] = 0;
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
			drawGLScene( );
			/* Draw it to the screen */
			//SDL_GL_SwapBuffers( );
			SDL_RenderPresent(displayRenderer);
			
			/* Gather our frames per second */
			Frames++;
			{
				GLint t = SDL_GetTicks();
				if (t - T0 >= 5000) {
					GLfloat seconds = (t - T0) / 1000.f;
					GLfloat fps = Frames / seconds;
					printf("%d frames in %g seconds = %g FPS\n", Frames, seconds, fps);
					T0 = t;
					Frames = 0;
				}
			}

		}
	}

	/* clean ourselves up and exit */
	Quit( 0 );

	/* Should never get here */
	return( 0 );
}
