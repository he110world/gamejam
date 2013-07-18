#include <GL/gl.h>
#include <SDL/SDL.h>

extern int keystat[1000];
extern int mousestat[10];
extern int mousex, mousey, mousexr, mouseyr;
extern void glBindTexture2D( const char* texname );
extern int xres, yres;

typedef struct Global_s
{
	int version;
} Global_t;

/* Here goes our drawing code */
int drawGLScene( GLvoid )
{
	glViewport(0,0,xres,yres);
    /* Clear The Screen And The Depth Buffer */
	glClearColor(0,0.5,0.5,1);	
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    /* Move Left 1.5 Units And Into The Screen 6.0 */
    glLoadIdentity();
    glTranslatef( -1.5f, 0.0f, -6.0f );

	float haha = keystat['f'] ? 0.0f : 1.0f;
	float haha2 = keystat[' '] ? 0.0f : 1.0f;
	float haha3 = mousestat[1] ? 1.0f : 0.0f;
    glBegin( GL_TRIANGLES );             /* Drawing Using Triangles       */
      glColor3f(  haha,  0.0f,  0.0f ); /* Red                           */
      glVertex3f(  0.0f,  1.0f,  0.0f ); /* Top Of Triangle               */
      glColor3f(   0.0f,  haha2,  0.0f ); /* Green                         */
      glVertex3f( -1.0f, -1.0f,  0.0f ); /* Left Of Triangle              */
      glColor3f(   0.0f,  0.0f,  1.0f ); /* Blue                          */
      glVertex3f(  haha3, -1.0f,  0.0f ); /* Right Of Triangle             */
    glEnd( );                            /* Finished Drawing The Triangle */

	static float x = 3.0f;
	static float y = 0.0f;
	if( mousestat[1] )
		x += mousexr / 100.0f;	
	if( mousestat[1] )
		y += -mouseyr / 100.0f;
	mousexr = 0;
	mouseyr = 0;
    /* Move Right 3 Units */
    glTranslatef( x, y, 0.0f );

   /* Set The Color To Blue One Time Only */
    glColor3f( 1.0f, 1.0f, 1.0f);

//	glTexture( "mud.png" );
//	glTexture( "http://www.google.com/logos/classicplus.png" );
//	glTexture( "http://www.google.com/images/errors/robot.png" );
	glTexture( "http://www.owoho.com/temp/demo3.jpg" );
    glBegin( GL_QUADS );                 /* Draw A Quad              */
	glTexCoord2f( 1, 0 );
      glVertex3f(  1.0f,  1.0f,  0.0f ); /* Top Right Of The Quad    */
	glTexCoord2f( 0, 0 );
      glVertex3f( -1.0f,  1.0f,  0.0f ); /* Top Left Of The Quad     */
	glTexCoord2f( 0, 1 );
      glVertex3f( -1.0f, -1.0f,  0.0f ); /* Bottom Left Of The Quad  */
	glTexCoord2f( 1, 1 );
      glVertex3f(  1.0f, -1.0f,  0.0f ); /* Bottom Right Of The Quad */
    glEnd( );                            /* Done Drawing The Quad    */

	glTexture( 0 );
    return( 0 );
}

