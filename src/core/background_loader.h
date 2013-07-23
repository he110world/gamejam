#ifndef background_loader_h__
#define background_loader_h__

#include "pthread.h"
#include <queue>
#include <string>
#include <set>

// filename/URL => raw data
class BackgroundLoader
{
public:
	BackgroundLoader();
	~BackgroundLoader();
	void load( const char* resname );

private:
	static void* loadThreadFunc( void* notused );

	static std::queue<std::string>	s_pendingRes_;
	static std::set<std::string>	s_uniqueRes_;

	pthread_t						threadID_;
	static bool						s_running_;
};

extern BackgroundLoader* g_backgroundLoader;

#endif // background_loader_h__
