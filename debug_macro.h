#ifndef __DEBUG_MACRO_H__
#define __DEBUG_MACRO_H__

#ifdef __DEBUG_ENABLED__

#define init_debug( DBG_GONFIG ) { __init_debug( DBG_GONFIG ); }

#define stop_debug() { __stop_debug(); }

#define debug( debug_level, args ... ) {													\
	if ( 0 == debug_ctx.initialized ) { __init_debug( NULL ); }											\
	if ( debug_ctx.__debug_level >= debug_level ) {													\
		char    ds[1024]; unsigned int pos = 0;													\
		if ( debug_ctx.__debug_mask & TIME_ENABLED ) {												\
			struct timeval tv; gettimeofday( &tv, NULL ); time_t t = tv.tv_sec; char *time_str = ctime( ( time_t * ) &t );			\
			time_str[strlen( time_str ) - 6] = 0; pos += sprintf( ds + pos, "[%s.%d]--", time_str, ( int )tv.tv_usec);			\
		}																	\
		if ( debug_ctx.__debug_mask & FILE_ENABLED ) { pos += sprintf( ds + pos, "[%s]", __FILE__ ); }						\
		if ( debug_ctx.__debug_mask & LINE_ENABLED ) { pos += sprintf( ds + pos, "[%d]", __LINE__ ); }						\
		if ( debug_ctx.__debug_mask & PID_ENABLED )  { pos += sprintf( ds + pos, "[%d]", getpid() ); }						\
		if ( debug_ctx.__debug_mask & FUNCTION_ENABLED ) {pos += sprintf( ds + pos, "%s()", __FUNCTION__); }					\
		if ( debug_ctx.__debug_mask & LOG_LEVEL_ENABLED )  { pos += sprintf( ds + pos, "[%s]", debug_level_type_str[ debug_level ]  ); }	\
		strcat(ds, ": "); sstrcat(ds, ""); printf( ds + pos, ##args ); strcat(ds, "\n");							\
		__debug( ds );																\
	}																		\
}

#else

#define init_debug( DBG_GONFIG ) {      }

#define debug( debug_level, args ... ){ }

#define stop_debug() { }

#endif

#endif // __DEBUG_MACRO_H__

