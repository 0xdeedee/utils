#ifndef __NET_DEBUG_H__
#define __NET_DEBUG_H__

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

typedef struct __attribute__ ((__packed__)) __attribute__ ((aligned (16))) __debug_cfg
{
	char				__debug_ip[16];
	int				__debug_port;
	int				__debug_level;
	unsigned char			__debug_mask:5;
} __debug_cfg_t;

typedef struct __attribute__ ((__packed__)) __attribute__ ((aligned (16))) __debug_ctx
{
	unsigned int			__initialized:1;
	int				__sock;
	struct sockaddr_in		__sock_addr;
	char				__ip[16];
	unsigned int 			__port;
	debug_type_e			__debug_type;
	unsigned char			__debug_mask:5;
	int				__debug_level;
} __debug_ctx_t;


__debug_ctx_t				debug_ctx;

static __inline__ __attribute__((always_inline)) void __init_debug( __debug_cfg_t *__in_debug_cfg )
{
	memset( &debug_ctx, 0, sizeof( debug_ctx ) );
	debug_ctx.__sock = -1;

	if ( NULL == __in_debug_cfg )
	{
		debug_ctx.__debug_mask = DEFAULT_DEBUG_MASK;
		debug_ctx.__debug_type = DEFAULT_DEBUG_TYPE;
	}

	if ( NULL != __in_debug_cfg )
		debug_ctx.__debug_level = __in_debug_cfg->__debug_level;
	else
		debug_ctx.__debug_level = DEFAULT_DEBUG_LEVEL;

	if ( ( NULL == __in_debug_cfg ) || ( 0 == __in_debug_cfg->__debug_port ) )
		debug_ctx.__port = DEFAULT_DEBUG_PORT;
	else
		debug_ctx.__port = __in_debug_cfg->__debug_port;

	if ( ( NULL == __in_debug_cfg ) || ( 0 == __in_debug_cfg->__debug_ip[0] ) )
		strcat( debug_ctx.__ip, DEFAULT_DEBUG_IP );
	else
		strcat( debug_ctx.__ip, __in_debug_cfg->__debug_ip );

	if ( -1 == ( debug_ctx.__sock = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP ) ) )
		printf( "Cannot open socket errno %d %s\n", errno, strerror(errno) );

	memset( ( char * ) &debug_ctx.__sock_addr, 0, sizeof( debug_ctx.__sock_addr ) );
	debug_ctx.__sock_addr.sin_family = AF_INET;
	debug_ctx.__sock_addr.sin_port = htons( debug_ctx.__port );

	if ( 0 == inet_aton(debug_ctx.__ip , &debug_ctx.__sock_addr.sin_addr ) )
	{
		printf( "inet_aton returns errno %d %s\n", errno, strerror( errno ) );
		close( debug_ctx.__sock );
		debug_ctx.__sock = -1;
	}

	debug_ctx.__initialized = 1;
}

static __inline__ __attribute__((always_inline)) void __debug( char *ds )
{
	if (		( 0 > debug_ctx.__sock )
		||	( 0 != debug_ctx.__initialized )
		||	( 0 > sendto( debug_ctx.__sock, ds, strlen( ds ), 0, ( struct sockaddr * )&debug_ctx.__sock_addr, sizeof( debug_ctx.__sock_addr ) ) ) )
	{
		printf( "Cannot send data socket [%d] idebug %s errno [%d] error [%s]\n",
				debug_ctx.__sock,
				( 0 != debug_ctx.__initialized )? "initiazed": "not initiazed",
				errno,
				strerror( errno ) );
	}
}

static __inline__ __attribute__((always_inline)) void __stop_debug()
{
	close( debug_ctx.__sock );
	debug_ctx.__sock = -1;
	debug_ctx.__initialized = 0;
}

#endif // __DEBUG_ENABLED__

#include "debug_macro.h"

#endif // __NET_DEBUG_H__


