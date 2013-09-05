#pragma once
#include "libtag.h"

#if PLATFORM == PLATFORM_WIN32
	#define __WINDOWS__ 1
	#define bcopy(s1,d1,len) CopyMemory(d1,s1,len)
	#define bzero(s1,len)	ZeroMemory(s1,len)
	#include <winsock2.h>
	#include <MSWSock.h>
	#pragma comment(lib,"ws2_32.lib")
#elif PLATFORM == PLATFORM_APPLE
	#define __MAC__ 1
	#include "sys/socket.h"
	#include <sys/time.h>
	#include "sys/poll.h"
	#include "sys/select.h"
	#include "sys/types.h"
	#include "netinet/in.h"
	#include "fcntl.h"
	#include "netdb.h"
	#include "signal.h"
	#include "memory.h"
	#include <arpa/inet.h>
	#include <pthread.h>
	#include "strings.h"
	#include "iconv.h"
	#include <strings.h>
	#include "unistd.h"
	#include "poll.h"
	#include <sys/event.h>
#else
	#include "sys/socket.h"
	#include "sys/epoll.h"
	#include <sys/time.h>
	#include "sys/poll.h"
	#include "sys/select.h"
	#include "sys/types.h"
	#include "netinet/in.h"
	#include "fcntl.h"
	#include "netdb.h"
	#include "signal.h"
	#include "memory.h"
	#include <arpa/inet.h>
	#include <pthread.h>
	#include "strings.h"
	#include "iconv.h"
	#include <strings.h>
	#include "unistd.h"
	#include "poll.h"
	#define __LINUX__ 1
#endif

#ifdef __USE_LUA_EXT
extern "C"{
	#include "lua.h"
}
#include "lua_tinker.h"
#endif

#if __LINUX__ || __MAC__
	#define DWORD unsigned int
	#define WORD unsigned short
	#define BYTE unsigned char
	#define DOUBLE unsigned long long
	#define SOUBLE long long
	#define QWORD unsigned long long
	#define SDWORD int
	#define LONG unsigned long long
    #define strtok_s strtok_r
#endif

//#define _USE_LUA_TINKER
//#define _USE_ZLIB_ 1
//#define _USE_LUA
//#define _USE_BASE64_ 1