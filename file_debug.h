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
#include <limits.h>


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

#define DEFAULT_DEBUG_TYPE		( DEBUG_TYPE_LOCAL )

#define DEFAULT_DEBUG_FILE		( "default_debug.log" )

#define DEFAULT_DEBUG_LEVEL		( 20 )

#define SIZE_1K				( 1024 * 1024 )
#define SIZE_1M				( 1024 * SIZE_1K )
#define SIZE_20M			( 20 * SIZE_1M )


#define PID_ENABLED			( 1 << 0 )
#define LINE_ENABLED			( 1 << 1 )
#define FILE_ENABLED			( 1 << 2 )
#define TIME_ENABLED			( 1 << 3 )

#define DEFAULT_DEBUG_MASK		( PID_ENABLED | LINE_ENABLED | FILE_ENABLED | TIME_ENABLED )

#define LogLevel_Info			( 0 )

__debug_ctx_t				debug_ctx;

static __inline__ void __init_debug( __debug_cfg_t *__in_debug_cfg )
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

static __inline__ void __debug( char *ds )
{
	if ( NULL == debug_ctx.fp )
		fputs( ds, debug_ctx.fp );
}

#ifdef __DEBUG_ENABLED__

#define init_debug( DBG_GONFIG ) { __init_debug( DBG_GONFIG ); }

#define stop_debug() { __stop_debug(); }

#define debug( debug_level, args ... ) {												\
	if ( 0 == debug_ctx.__initialized ) { __init_debug( NULL ); }									\
	if ( debug_ctx.__debug_level >= debug_level ) {											\
		char	ds[1024]; unsigned int pos = 0;											\
		if ( debug_ctx.__debug_mask & TIME_ENABLED ) { 										\
			struct timeval tv; gettimeofday( &tv, NULL ); time_t t = tv.tv_sec; char *time_str = ctime( ( time_t * ) &t );	\
			time_str[strlen( time_str ) - 6] = 0; pos += sprintf( ds + pos, "[%s.%d]--", time_str, ( int )tv.tv_usec);	\
		}															\
		if ( debug_ctx.__debug_mask & FILE_ENABLED ) { pos += sprintf( ds + pos, "[%s]", __FILE__ ); }				\
		if ( debug_ctx.__debug_mask & LINE_ENABLED ) { pos += sprintf( ds + pos, "[%d]", __LINE__ ); }				\
		if ( debug_ctx.__debug_mask & PID_ENABLED )  { pos += sprintf( ds + pos, "[%d]", getpid() ); }				\
		pos += sprintf( ds + pos, "%s():", __FUNCTION__);									\
		sprintf( ds + pos, ##args ); strcat(ds, "\n");										\
		__debug( ds );														\
	}																\
}
#else
#define init_debug( DBG_GONFIG ) {	}
#define debug( debug_level, args ... ){	}
#define stop_debug() { }
#endif


#endif // __DEBUG_H__



