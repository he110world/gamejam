#ifndef platform_h__
#define platform_h__

// OS definitions
#if defined(__APPLE__) || defined(__MACOS__)
	#if (defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__) || defined(__IPHONE_OS_VERSION_MIN_REQUIRED))
		#define GJ_OS_IOS
	#else
		#define GL_OS_OSX
	#endif
#elif defined(WIN64) || defined(_WIN64) || defined(__WIN64__)
	#define GJ_OS_WIN64
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
	#define GJ_OS_WIN32
#elif defined(__linux__) || defined(__linux)
	#define GJ_OS_LINUX
#elif defined(ANDROID)
	#define GJ_OS_ANDROID
#else
	#define GJ_OS_UNSUPPORTED
#endif

#if defined(GJ_OS_WIN32) || defined(GJ_OS_WIN64)
	#define GJ_OS_WINDOWS
#endif

// CPU definitions
#if defined(__x86_64__) || defined(GJ_OS_WIN64)
	#define GJ_CPU_X86_64
#elif defined(__i386__) || defined(GJ_OS_WIN32)
	#define GJ_CPU_X86
#elif defined(__ARM__)
	#define GJ_CPU_ARM
#else
	#define GJ_CPU_UNSUPPORTED
#endif

#endif // platform_h__
