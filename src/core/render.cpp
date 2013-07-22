#include <GLES2/gl2.h>
#include <map>
#include <assert.h>
#include "render.h"
#include "model.h"

void modelLoad( int modelid, const char* modelname )
{

}

void modelCreate( int modelid, int numverts )
{
	g_modelManager->create(modelid, numverts);
}

void modelDestroy( int modelid )
{
	g_modelManager->destroy(modelid);
}

void modelDraw( int modelid )
{
	g_modelManager->draw(modelid);
}

void modelBeginUpdate( int modelid )
{
	g_modelManager->beginUpdate(modelid);
}

void modelEndUpdate()
{
	g_modelManager->endUpdate();
}

void modelSetQuad( int v1, int v2, int v3, int v4 )
{
	g_modelManager->setQuad(v1,v2,v3,v4);
}

void modelSetTri( int v1, int v2, int v3 )
{
	g_modelManager->setTri(v1,v2,v3);
}

void modelVertex( int vertid, float x, float y, float z )
{
	g_modelManager->updateVertex(vertid,x,y,z);
}

void modelTexCoord( int vertid, float u, float v )
{
	g_modelManager->updateTexCoord(vertid,u,v);
}

void modelNormal( int vertid, float nx, float ny, float nz )
{
	g_modelManager->updateNormal(vertid,nx,ny,nz);
}

void renderInit()
{
	if( !g_modelManager )
	{
		g_modelManager = new ModelManager;
	}
}

void renderFini()
{
	if( g_modelManager )
	{
		delete g_modelManager;
	}
}