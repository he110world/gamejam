#include <map>
#include <assert.h>
#include "render.h"
#include "texture.h"
#include "shader.h"
#include "model.h"

void renderInit()
{
	if( !g_modelManager )
	{
		g_modelManager = new ModelManager;
	}

	if( !g_textureManager )
	{
		g_textureManager = new TextureManager;
	}

	if( !g_shaderManager )
	{
		g_shaderManager = new ShaderManager;
	}
}

void renderFini()
{
	if( g_modelManager )
	{
		delete g_modelManager;
		g_modelManager = NULL;
	}

	if( g_textureManager )
	{
		delete g_textureManager;
		g_textureManager = NULL;
	}

	if( g_shaderManager )
	{
		delete g_shaderManager;
		g_shaderManager = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////
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

void modelTexture( int modelid, const char* texname )
{
	g_modelManager->setTexture(modelid, texname);
}

void modelShader( int modelid, const char* vs, const char* ps )
{
	g_modelManager->setShader(modelid, vs, ps);
}

//////////////////////////////////////////////////////////////////////////
void modelBeginUpdateMesh( int modelid )
{
	g_modelManager->beginUpdate(modelid);
}

void modelEndUpdateMesh()
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