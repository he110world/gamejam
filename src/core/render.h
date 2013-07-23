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
void modelTexture( int modelid, const char* texname );
void modelShader( int modelid, const char* vs, const char* ps );

// update model geometry
void modelBeginUpdateMesh( int modelid );
void modelEndUpdateMesh();
void modelSetQuad( int v1, int v2, int v3, int v4 );
void modelSetTri( int v1, int v2, int v3 );
void modelVertex( int vertid, float x, float y, float z );
void modelTexCoord( int vertid, float u, float v );
void modelNormal( int vertid, float nx, float ny, float nz );

#endif // render_h__
