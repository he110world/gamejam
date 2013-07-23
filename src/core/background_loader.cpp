#include "background_loader.h"
#include "pthread.h"

BackgroundLoader* g_backgroundLoader = NULL;
bool BackgroundLoader::s_running_ = true;
static pthread_mutex_t s_resLock = PTHREAD_MUTEX_INITIALIZER;
std::queue<std::string> BackgroundLoader::s_pendingRes_;
std::set<std::string> BackgroundLoader::s_uniqueRes_;

BackgroundLoader::BackgroundLoader()
{
	pthread_mutex_init(&s_resLock, NULL);
	pthread_create(&threadID_, NULL, loadThreadFunc, NULL);
}

BackgroundLoader::~BackgroundLoader()
{

}

void BackgroundLoader::load( const char* resname )
{
	pthread_mutex_lock(&s_resLock);
	std::set<std::string>::iterator it = s_uniqueRes_.find(resname);
	if( it == s_uniqueRes_.end() )
	{
		// not in res queue
		s_uniqueRes_.insert(resname);
		s_pendingRes_.push(resname);
	}
	pthread_mutex_unlock(&s_resLock);
}

void* BackgroundLoader::loadThreadFunc( void* notused )
{
	while(s_running_)
	{
		pthread_mutex_lock(&s_resLock);
		if( !s_pendingRes_.empty() )
		{
			std::string resname = s_pendingRes_.front();

			// load file
			s_pendingRes_.pop();
		}
		pthread_mutex_unlock(&s_resLock);
	}
	return NULL;
}
