#ifndef model_h__
#define model_h__

#include <map>

class Model;
class ModelManager
{
public:
	ModelManager();
	~ModelManager();

	void loadObj( int id );
	void loadMd5( int id );
	void create( int id, int numverts );
	void destroy( int id );
	void draw( int id );
	void beginUpdate( int id );
	void endUpdate();
	void setQuad( int v1, int v2, int v3, int v4 );
	void setTri( int v1, int v2, int v3 );
	void updateVertex( int vertid, float x, float y, float z );
	void updateTexCoord( int vertid, float u, float v );
	void updateNormal( int vertid, float nx, float ny, float nz );
	Model* get( int id )
	{
		std::map<int, Model*>::iterator it = models_.find(id);
		if( it != models_.end() )
		{
			return it->second;
		}
		return NULL;
	}

private:
	std::map<int, Model*>	models_;
	Model*					current_;
};

extern ModelManager* g_modelManager;

#endif // model_h__
