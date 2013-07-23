#ifndef texture_h__
#define texture_h__

#include <string>
#include <map>

struct Texture
{
	Texture( const char* name );
	~Texture();

	short			width;
	short			height;
	size_t			numBytes;
	unsigned int	texID;
	int				refCount;
};

// LRU cache
class TextureCache
{
public:
	TextureCache();
	~TextureCache();

	void setCapacity( size_t size )
	{
		capacity_ = size;
	}
	void push( const char* texname, Texture* tex );
	Texture* pop(const char* texname);

private:
	size_t		capacity_;
	size_t		usedSize_;
};

// textures are ref-counted
class TextureManager
{
public:
	unsigned int incRef( const char* texname );
	void decRef( const char* texname );

private:
	std::map<std::string, Texture*>		textures_;
	TextureCache						cache_;
};

#define UNDEFINED_TEXTURE	0
extern TextureManager* g_textureManager;

#endif // texture_h__
