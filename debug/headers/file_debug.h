#ifndef __FILE_DEBUG_H__
#define __FILE_DEBUG_H__

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

#define DEFAULT_DEBUG_FILE		( "default_debug.log" )

// debug everything
#define DEFAULT_DEBUG_LEVEL		( 20 )

#define SIZE_1K				( 1024 * 1024 )
#define SIZE_1M				( 1024 * SIZE_1K )
#define SIZE_20M			( 20 * SIZE_1M )

// 1 to enable feature 0 to disable 
#define PID_ENABLED			( 1 << 0 )
#define LINE_ENABLED			( 1 << 1 )
#define FILE_ENABLED			( 1 << 2 )
#define TIME_ENABLED			( 1 << 3 )
#define LOG_LEVEL_ENABLED		( 1 << 4 )
#define FUNCTION_ENABLED		( 1 << 5 )

#define DEFAULT_DEBUG_MASK		( PID_ENABLED | LINE_ENABLED | FILE_ENABLED | TIME_ENABLED )

#define MAX_DEBUG_LEVEL			( 40 )
#define MAX_DEBUG_FILES			( 20 )
#define LogLevel_Info			( 0 )

typedef struct __attribute__ ((__packed__)) __attribute__ ((aligned (16))) __debug_cfg
{
	char				__debug_filename[PATH_MAX - 11];
	int				__debug_level;
	unsigned char			__debug_mask:5;
	unsigned int			__debug_file_size;
} __debug_cfg_t;

typedef struct __attribute__ ((__packed__)) __attribute__ ((aligned (16))) __debug_ctx
{
	unsigned int			__initialized:1;
	FILE				*__fp;
	char				__fn[PATH_MAX];
	char				__debug_filename[PATH_MAX - 11];
	debug_type_e			__debug_type;
	unsigned char			__debug_mask:5;
	int				__debug_level;
	unsigned int			__debug_file_size;
	unsigned int			__debug_file_count;
} __debug_ctx_t;

__debug_ctx_t				debug_ctx;

static __inline__ __attribute__((always_inline)) void __init_debug( __debug_cfg_t *__in_debug_cfg )
{
	struct stat		st;

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
		strcat( debug_ctx.__debug_filename, DEFAULT_DEBUG_FILE);
	else
		strcat( debug_ctx.__debug_filename, __in_debug_cfg->__debug_filename );
	snprintf( debug_ctx.__fn, sizeof( debug_ctx.__fn ) - 1, "debug_%s_%d_%d.txt", debug_ctx.__debug_filename, getpid(), debug_ctx.__debug_file_count++ );


	if ( NULL == ( debug_ctx.__fp = fopen( debug_ctx.__fn, "a+" ) ) )
	{
		printf( "Cannot open file %s errno %d error %s\n", debug_ctx.__fn, errno, str_error( errno ) );
		return;
	}

	memset( &st, 0, sizeof( st ) );
	if ( 0 == stat( debug_ctx.__fn, &st ) )
		debug_ctx.__debug_file_size = st.st_size;
	else
		debug_ctx.__debug_file_size = 0;

	debug_ctx.__debug_file_size = 0;
	debug_ctx.__initialized = 1;
}

static __inline__ __attribute__((always_inline)) void __debug( char *ds )
{
	if ( NULL != debug_ctx.__fp )
	{
		debug_ctx.__debug_file_size += fputs( ds, debug_ctx.__fp );
#ifdef __DEBUG_FILE_ROTATION_ENABLED__
		if ( debug_ctx.__debug_file_size > SIZE_20M )
		{
			snprintf( debug_ctx.__fn, sizeof( debug_ctx.__fn ) - 1, "debug_%s_%d_%d.txt", debug_ctx.__debug_filename, getpid(), debug_ctx.__debug_file_count++ );
		        if ( NULL == ( debug_ctx.__fp = fopen( debug_ctx.__fn, "w+" ) ) )
			{
				close( debug_ctx.__fp );
				printf( "Cannot open file %s errno %d error %s\n", debug_ctx.__fn, errno, str_error( errno ) );
				debug_ctx.__initialized = 0;
				return;
			}
		}
#endif // __DEBUG_FILE_ROTATION_ENABLED__
	}
}

static __inline__ __attribute__((always_inline)) void __stop_debug()
{
	close( debug_ctx.__fp );
	debug_ctx.__fp = NULL;
	debug_ctx.__initialized = 0;
}

#endif // __DEBUG_ENABLED__

#include "debug_macro.h"

#endif // __FILE_DEBUG_H__



