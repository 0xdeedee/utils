#ifndef __LOCAL_DEBUG_H__
#define __LOCAL_DEBUG_H__

#ifdef __DEBUG_ENABLED__

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <limits.h>


typedef enum
{
	DEBUG_TYPENONE			= 0,
	DEBUG_TYPE_NETWORK		= 1,
	DEBUG_TYPE_LOCAL		= 2,
} debug_type_e;

typedef struct __attribute__ ((__packed__)) __attribute__ ((aligned (16))) __debug_cfg
{
	int				__debug_level;
	unsigned char			__debug_mask:5;
} __debug_cfg_t;

typedef struct __debug_ctx
{
	unsigned int			__initialized:1;
	debug_type_e			__debug_type;
	unsigned char			__debug_mask:5;
	int				__debug_level;
} __debug_ctx_t;

#define DEFAULT_DEBUG_TYPE		( DEBUG_TYPE_LOCAL )

#define DEFAULT_DEBUG_MASK		( PID_ENABLED | LINE_ENABLED | FILE_ENABLED | TIME_ENABLED )

#define DEFAULT_DEBUG_LEVEL		( 20 )

#define SIZE_10K			( 1024 * 1024 )

#define PID_ENABLED			( 0 << 0 )
#define LINE_ENABLED			( 1 << 1 )
#define FILE_ENABLED			( 1 << 2 )
#define TIME_ENABLED			( 1 << 3 )

#define LogLevel_Info			( 0 )

__debug_ctx_t				debug_ctx;

static __inline__ __attribute__((always_inline)) void __init_debug( __debug_cfg_t *__in_debug_cfg )
{
	memset( &debug_ctx, 0, sizeof( debug_ctx ) );

	if ( NULL == __in_debug_cfg )
	{
		debug_ctx.__debug_mask = DEFAULT_DEBUG_MASK;
		debug_ctx.__debug_type = DEFAULT_DEBUG_TYPE;
	}

	if ( NULL != __in_debug_cfg )
		debug_ctx.__debug_level = __in_debug_cfg->__debug_level;
	else
		debug_ctx.__debug_level = DEFAULT_DEBUG_LEVEL;

	debug_ctx.initialized = 1;
}

static __inline__ __attribute__((always_inline)) void __debug( char *ds )
{
	printf( "%s", ds );
}

static __inline__ __attribute__((always_inline)) void __stop_debug()
{

}

#endif // __DEBUG_ENABLED__

#include "debug_macro.h"

#endif // __LOCAL_DEBUG_H__



