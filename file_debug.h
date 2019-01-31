#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>


#define DEFAULT_DEBUG_PORT 		( 50505 )
#define DEFAULT_DEBUG_IP		( "127.0.0.1" )

// debug everything
#define DEFAULT_DEBUG_LEVEL		( 20 )

#define SIZE_10K			( 1024 * 1024 )

// 1 to enable feature 0 to disable 
#define PID_ENABLED			( 1 << 0 )
#define LINE_ENABLED			( 1 << 1 )
#define FILE_ENABLED			( 1 << 2 )
#define TIME_ENABLED			( 1 << 3 )
#define LOG_LEVEL_ENABLED		( 1 << 4 )
#define FUNCTION_ENABLED		( 1 << 5 )

#define DEFAULT_DEBUG_MASK		( PID_ENABLED | LINE_ENABLED | FILE_ENABLED | TIME_ENABLED )

#define MAX_DEBUG_LEVEL			( 40 )
#define LogLevel_Info			( 0 )

static char		debug_level_type_str[4][MAX_DEBUG_LEVEL] = 
{
	{ "ERR" },
	{ "ERR" },
	{ "ERR" },
	{ "ERR" },
	{ "ERR" },
	{ "WARNING" },
	{ "WARNING" },
	{ "WARNING" },
	{ "WARNING" },
	{ "WARNING" },
	{ "WARNING" },
	{ "WARNING" },
	{ "WARNING" },
	{ "WARNING" },
	{ "WARNING" },
	{ "WARNING" },
	{ "WARNING" },
	{ "WARNING" },
	{ "WARNING" },
	{ "WARNING" },
	{ "INFO" },
	{ "INFO" },
	{ "INFO" },
	{ "INFO" },
	{ "INFO" },
	{ "INFO" },
	{ "INFO" },
	{ "INFO" },
	{ "INFO" },
	{ "INFO" },
	{ "INFO" },
	{ "INFO" },
	{ "INFO" },
	{ "INFO" },
	{ "INFO" },
	{ "INFO" },
	{ "INFO" },
	{ "INFO" },
	{ "INFO" },
	{ "INFO" },
};


typedef struct __attribute__ ((__packed__)) __attribute__ ((aligned (16))) __debug_cfg
{
	char				__debug_filename[PATH_MAX - 11];
	int				__debug_level;
	unsigned char			__debug_mask:5;
	unsigned int			__debug_file_size;
} __debug_cfg_t;

typedef struct __debug_ctx
{
	unsigned int			__initialized:1;
	FILE				*__fp;
	char				__fn[PATH_MAX];
	debug_type_e			__debug_type;
	unsigned char			__debug_mask:5;
	int				__debug_level;
	unsigned int			__debug_file_size;
} __debug_ctx_t;


#define DEFAULT_DEBUG_FILE		( "default_debug.log" )



__debug_ctx_t				debug_ctx;

static __inline__ __attribute__((always_inline)) void __init_debug( __debug_cfg_t *__in_debug_cfg )
{
	char			__debug_filename[PATH_MAX - 11];

	memset( &debug_ctx, 0, sizeof( debug_ctx ) );
	memset( __debug_filename, 0, sizeof( __debug_filename ) );

	if ( NULL == __in_debug_cfg )
	{
		debug_ctx.__debug_mask = DEFAULT_DEBUG_MASK;
		debug_ctx.__debug_type = DEFAULT_DEBUG_TYPE;
	}

	if ( NULL != __in_debug_cfg )
		debug_ctx.__debug_level = __in_debug_cfg->__debug_level;
	else
		debug_ctx.__debug_level = DEFAULT_DEBUG_LEVEL;


	if ( ( NULL == __in_debug_cfg ) || ( 0 == __in_debug_cfg->__debug_filename[0] ) )
		strcat( __debug_filename, DEFAULT_DEBUG_FILE);
	else
		strcat( __debug_filename, __in_debug_cfg->__debug_filename );

	snprintf( debug_ctx.__fn, sizeof( debug_ctx.__fn ) - 1, "debug_%s_%d.txt", __debug_filename, getpid() );

	if ( NULL == ( debug_ctx.__fp = fopen( debug_ctx.__fn, "a+" ) ) )
	{
		printf( "Cannot open file %s errno %d error %s\n", debug_ctx.__fn, errno, str_error( errno ) );
		return;
	}

	debug_ctx.__initialized = 1;
}

static __inline__ __attribute__((always_inline)) void __debug( char *ds )
{
	if ( NULL == debug_ctx.fp )
		fputs( ds, debug_ctx.fp );
}

static __inline__ __attribute__((always_inline)) void __stop_debug()
{
	close( debug_ctx.__sock );
	debug_ctx.__sock = -1;
	debug_ctx.__initialized = 0;
}

#include "debug_macro.h"

#endif // __DEBUG_H__



