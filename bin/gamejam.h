#ifndef GAMEJAM_H
#define GAMEJAM_H

// gamejam API list

// GUI
#define RGBA2HEX(r,g,b,a) ((r) | (g << 8) | (b << 16) | (a << 24))

void guiBeginFrame(int mx, int my, unsigned char mbut, int scroll);
void guiEndFrame();

int guiBeginScrollArea(const char* name, int x, int y, int w, int h, int* scroll);
void guiEndScrollArea();

void guiIndent();
void guiUnindent();
void guiSeparator();
void guiSeparatorLine();

int guiButton(const char* text, int enabled);
int guiItem(const char* text, int enabled);
int guiCheck(const char* text, int checked, int enabled);
int guiCollapse(const char* text, const char* subtext, int checked, int enabled);
void guiLabel(const char* text);
void guiValue(const char* text);
int guiSlider(const char* text, float* val, float vmin, float vmax, float vinc, int enabled);

void guiDrawText(int x, int y, int align, const char* text, unsigned int color);
void guiDrawLine(float x0, float y0, float x1, float y1, float r, unsigned int color);
void guiDrawRoundedRect(float x, float y, float w, float h, float r, unsigned int color);
void guiDrawRect(float x, float y, float w, float h, unsigned int color);

// basic model functions
void modelLoad( int modelid, const char* modelname );	// if modelid already exists, nothing happens
void modelCreate( int modelid, int numverts );			// if modelid already exists, nothing happens
void modelDestroy( int modelid );
void modelDraw( int modelid );

// update model
void modelBeginUpdate( int modelid );
void modelEndUpdate();
void modelSetQuad( int v1, int v2, int v3, int v4 );
void modelSetTri( int v1, int v2, int v3 );
void modelVertex( int vertid, float x, float y, float z );
void modelTexCoord( int vertid, float u, float v );
void modelNormal( int vertid, float nx, float ny, float nz );

// 3d cursor (current translation/rotation/scale)
void moveTo( float x, float y, float z );
void rotateTo( float axisx, float axisy, float axisz, float angle );
void scaleTo( float x, float y, float z );

#endif // GAMEJAM_H