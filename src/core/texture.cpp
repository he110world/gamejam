#include "texture.h"
#include "stb_image.h"
#include <pthread.h>
#include <curl/curl.h>

#define DEFAULT_CACHE_SIZE	20*1024*1024

TextureManager* g_textureManager = NULL;

unsigned int TextureManager::incRef( const char* texname )
{
	// already loaded?
	Texture* tex = NULL;
	std::map<std::string, Texture*>::iterator it = textures_.find(texname);
	if( it != textures_.end() )
	{
		tex = it->second;
	}

	// cached?
	if( !tex )
	{
		tex = cache_.pop(texname);
	}

	if( tex )
	{
		tex->refCount++;
	}
	else
	{
		tex = new Texture(texname);
		textures_[texname] = tex;
	}
	return tex->texID;
}

void TextureManager::decRef( const char* texname )
{
	std::map<std::string, Texture*>::iterator it = textures_.find(texname);
	if( it != textures_.end() )
	{
		Texture* tex = it->second;	
		tex->refCount--;

		if( tex->refCount<= 0 )
		{
			textures_.erase(it);
			cache_.push(texname, tex);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
Texture::Texture( const char* name )
{
	// background loading
}

Texture::~Texture()
{

}

//////////////////////////////////////////////////////////////////////////
TextureCache::TextureCache()
: usedSize_(0)
, capacity_(DEFAULT_CACHE_SIZE)
{

}

TextureCache::~TextureCache()
{

}

void TextureCache::push( const char* texname, Texture* tex )
{
	// already cached?

	// 
	usedSize_ += tex->numBytes;

	// cache full
	if( usedSize_ > capacity_ )
	{
		// LRU
	}

	tex->refCount = 0;
}

Texture* TextureCache::pop( const char* texname )
{
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
#if 0
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

static size_t curlDownloadImage_cb(void *contents, size_t size, size_t nmemb, void *userp)
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

void loadTexture_ThreadMain()
{

}
#endif