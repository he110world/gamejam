#include "model.h"
#include "cursor3d.h"
#include <GLES2/gl2.h>
#include <vector>

//////////////////////////////////////////////////////////////////////////
struct ModelVertex
{
	float x,y,z;
};

struct ModelTexCoord
{
	float u,v;
};

struct ModelNormal
{
	float nx,ny,nz;
};

class Model
{
public:
	Model( int numverts );
	~Model();

	void setQuad( int v1, int v2, int v3, int v4 );
	void setTri( int v1, int v2, int v3 );
	void updateVertex( int vertid, float x, float y, float z );
	void updateTexCoord( int vertid, float u, float v );
	void updateNormal( int vertid, float nx, float ny, float nz );
	void draw();
	void update();

private:
	int							numVerts_;

	// software buffers
	std::vector<ModelVertex>	verts_;
	std::vector<ModelTexCoord>	texcoords_;
	std::vector<ModelNormal>	normals_;

	bool						vertexDirty_;
	bool						texcoordDirty_;
	bool						normalDirty_;
	bool						indexDirty_;

	// hardware buffers
};

Model::Model( int numverts )
: numVerts_(numverts)
, vertexDirty_(false)
, texcoordDirty_(false)
, normalDirty_(false)
, indexDirty_(false)
{
	verts_.resize(numverts);
	texcoords_.resize(numverts);
	normals_.resize(numverts);
}

Model::~Model()
{

}

void Model::setQuad( int v1, int v2, int v3, int v4 )
{
	indexDirty_ = true;
}

void Model::setTri( int v1, int v2, int v3 )
{
	indexDirty_ = true;
}

void Model::updateVertex( int vertid, float x, float y, float z )
{
	if( vertid < numVerts_ )
	{
		ModelVertex& vert = verts_[vertid];
		if( vertexDirty_ || vert.x!=x || vert.y!=y || vert.z!=z )	// only update if 1) already dirty or 2) verts are different => static model won't update
		{
			vert.x = x;
			vert.y = y;
			vert.z = z;
			vertexDirty_ = true;
		}
	}
}

void Model::updateTexCoord( int vertid, float u, float v )
{
	if( vertid < numVerts_ )
	{
		ModelTexCoord& tc = texcoords_[vertid];
		if( texcoordDirty_ || tc.u!=u || tc.v!=v )
		{
			tc.u = u;
			tc.v = v;
			texcoordDirty_ = true;
		}
	}
}

void Model::updateNormal( int vertid, float nx, float ny, float nz )
{
	if( vertid < numVerts_ )
	{
		ModelNormal& normal = normals_[vertid];
		if( normalDirty_ || normal.nx!=nx || normal.ny!=ny || normal.nz!=nz )
		{
			normal.nx = nx;
			normal.ny = ny;
			normal.nz = nz;
			normalDirty_ = true;
		}
	}
}

void Model::draw()
{
	// get matrix from cursor3d
}

void Model::update()
{
}

//////////////////////////////////////////////////////////////////////////
ModelManager* g_modelManager = NULL;

ModelManager::ModelManager()
: current_(NULL)
{
	// init shaders
}

ModelManager::~ModelManager()
{

}
void ModelManager::loadObj( int id )
{
	if( get(id) )
	{
		return;
	}
}

void ModelManager::loadMd5( int id )
{
	if( get(id) )
	{
		return;
	}
}

void ModelManager::create( int id, int numverts )
{
	if( get(id) )
	{
		return;
	}

	Model* model = new Model(numverts);
	models_[id] = model;
}

void ModelManager::destroy( int id )
{
	std::map<int, Model*>::iterator it = models_.find(id);
	if( it != models_.end() )
	{
		Model* model = it->second;

		// don't forget to check if the model to delete is the current model
		if( current_ == model )
		{
			current_ = NULL;
		}

		delete model;
		models_.erase(it);
	}
}

void ModelManager::draw( int id )
{
	Model* model = get(id);
	if( model )
	{
		model->draw();
	}
}

void ModelManager::beginUpdate( int id )
{
	current_ = get(id);
}

void ModelManager::endUpdate()
{
	if( current_ )
	{
		current_->update();
		current_ = NULL;
	}
}

void ModelManager::setQuad( int v1, int v2, int v3, int v4 )
{
	if( current_ )
	{
		current_->setQuad(v1,v2,v3,v4);
	}
}

void ModelManager::setTri( int v1, int v2, int v3 )
{
	if( current_ )
	{
		current_->setTri(v1,v2,v3);
	}
}

void ModelManager::updateVertex( int vertid, float x, float y, float z )
{
	if( current_ )
	{
		current_->updateVertex(vertid,x,y,z);
	}
}

void ModelManager::updateTexCoord( int vertid, float u, float v )
{
	if( current_ )
	{
		current_->updateTexCoord(vertid,u,v);
	}
}

void ModelManager::updateNormal( int vertid, float nx, float ny, float nz )
{
	if( current_ )
	{
		current_->updateNormal(vertid,nx,ny,nz);
	}
}
