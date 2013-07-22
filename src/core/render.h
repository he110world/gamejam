#ifndef render_h__
#define render_h__

// render system functions
void renderInit();
void renderFini();

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

#endif // render_h__
