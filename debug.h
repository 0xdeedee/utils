#ifndef __DEBUG_H__
#define __DEBUG_H__

#define __DEBUG_ENABLED__
//#undef __DEBUG_ENABLED__

#define __DEBUG_FILE_ENABLED__
//#undef __DEBUG_FILE_ENABLED__

#define __DEBUG_NET_ENABLED__
//undef __DEBUG_NET_ENABLED__

#if defined ( __DEBUG_FILE_ENABLED__ )

#include "headers/file_debug.h"

#elif defined ( __DEBUG_NET_ENABLED__)

#include "headers/net_debug.h"

#else

#include "headers/local_debug.h"

#endif

#endif // __DEBUG_H__



