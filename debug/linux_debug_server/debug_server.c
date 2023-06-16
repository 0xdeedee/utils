#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#include <errno.h>

#include "list.h"

#define INVALID_SOCKET				( -1 )
#define SOCKET_ERROR				( -1 )
#define SOCKET					int


typedef struct thread_data
{
	list_t			head;
	char			storage_dir[FILENAME_MAX - 64];
	char			app_name[64];
	unsigned short		port_no;
	pthread_t		tid;
	unsigned int		exit:1;
} thread_data_t;

typedef struct file_data
{
	list_t		list;
	char		ip[17];
	FILE		*fp;
} file_data_t;

static FILE *open_file(char *fn)
{
	FILE	*fp = fopen(fn, "a+");

	if ( NULL == fp )
		printf( "Cannot open file [%s] error [%s]\n", fn, strerror( errno ) );
	return fp;
}

static FILE *check_ip( thread_data_t *td, char *ip )
{
	file_data_t		*f = NULL;
	list_t			*l;

	if ( LIST_EMPTY == list_empty( &td->head ) )
		return NULL;
	list_for(l, &td->head)
	{
		f = list_entry( l, file_data_t, list);
		if ( 0 == strncmp( ip, f->ip, strlen( ip ) ) )
		{
			return f->fp;
		}
	}
	return NULL;
}

static char *generate_fn( char *path, char *app_name, char *ip, unsigned short portno )
{
	char			fn[FILENAME_MAX];
	struct stat		st;

	if ( 0 == stat( path, &st ) )
	{
		if ( S_IFDIR == ( st.st_mode & S_IFMT ) )
		{
			snprintf( fn, sizeof( fn ) - 1, "%s/%d/%s_%s_debug.log", path, portno, app_name, ip);
		}
		else
		{
			printf( "Cannot create debug is directory [%s]\n", path );
			return NULL;
		}
	}
	else
	{
		if ( ENOENT == errno )
		{
			char		dir[FILENAME_MAX];
			snprintf( dir, FILENAME_MAX - 1, "%s/%d", path, portno );
			if ( mkdir( dir, S_IRWXU | S_IXGRP | S_IRGRP | S_IROTH | S_IXOTH ) != 0)
			{
				printf( "Cannot create debug storage directory [%s]\n", strerror( errno ) );
				return NULL;
			}
			snprintf( fn, sizeof( fn ) - 1, "%s/%d/%s_%s_debug.log", path, portno, app_name, ip);
		}
		else
		{
			printf( "Cannot stat debug storage directory [%s]\n", strerror( errno ) );
			return NULL;
		}
	}

	return strdup( fn );
}

static void __write( thread_data_t *td, char *a, char *b)
{
	FILE			*fp = check_ip(td, a);
	char			*fn = NULL;
	file_data_t		*f = NULL;

	if ( NULL == fp )
	{
		if ( NULL != (fn = generate_fn( td->storage_dir, td->app_name, a, td->port_no ) ) )
		{
			fp = open_file( fn );
			free( fn );
			if ( NULL == fp )
			{
				return;
			}
			if ( NULL == ( f = ( file_data_t * )calloc(1, sizeof( file_data_t ) ) ) )
			{
				fclose(fp);
				return;
			}
			f->fp = fp;
			strcat(f->ip, a);
			list_append( &f->list, &td->head );
		}
	}
	
	fputs(b, fp);
}

static int init_sock( unsigned short portno )
{
	SOCKET				__sock = INVALID_SOCKET;
	struct sockaddr_in		__addr;

	if ( INVALID_SOCKET == ( __sock = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP ) ) )
	{
		printf( "Open socket failed with error %s\n", strerror( errno ) );
		return INVALID_SOCKET;
	}

	__addr.sin_family = AF_INET;
	__addr.sin_port = htons( portno );
	__addr.sin_addr.s_addr = htonl( INADDR_ANY );

	if ( 0 !=  bind( __sock, ( struct sockaddr * )& __addr, sizeof( __addr ) ) )
	{
		printf( "bind failed with error %s\n", strerror( errno ) );
		close ( __sock );
		return INVALID_SOCKET;
	}

	return __sock;
}

void *write_to_file( void *input_data )
{
	thread_data_t				*__input_data = ( thread_data_t * )input_data;
	SOCKET					sock;
	char					b[1024];
	int					bsz = 1024;
	struct sockaddr_in			a;
	int					asz = sizeof( struct sockaddr_in );

	if ( NULL == __input_data )
	{
		printf( "Cannot start debug thread missing input data\n" );
		return NULL;
	}

	if ( INVALID_SOCKET == ( sock = init_sock( __input_data->port_no ) ) )
	{
		return NULL;
	}

	list_init( &__input_data->head );
	printf( "Thread %ld receiving datagrams from %s on port %d...\n", 
			__input_data->tid, __input_data->app_name, __input_data->port_no );
	while ( 1 )
	{
		memset( b, 0, bsz );
		if ( SOCKET_ERROR != recvfrom( sock, b, bsz, 0, ( struct sockaddr * )&a, ( socklen_t * )&asz ) )
		{
			__write( __input_data, inet_ntoa( a.sin_addr ), b );
		}
		else
		{
			printf( "recvfrom failed with error %s\n", strerror( errno ) );
		}
	}
	printf( "Finished receiving. Closing socket.\n" );

	if ( SOCKET_ERROR == close( sock ) )
	{
		printf( "Close socket failed with error %s\n", strerror( errno ) );
		return NULL;
	}

	printf( "Exiting...\n" );
	return NULL;
}


//        char                    storage_dir[FILENAME_MAX - 64];
//        char                    app_name[64];
//        unsigned short          port_no;
//        pthread_t               tid;
//        unsigned int            exit:1;



static void start_cli( thread_data_t *cfg, unsigned int cfg_sz )
{
}


int main(int argc, char **argv)
{
	thread_data_t		cfg[1];

	memset( cfg, 0, sizeof( cfg ) );
	strcat( cfg[0].storage_dir, "./" );
	strcat( cfg[0].app_name, "app");
	cfg[0].port_no = 5005;
	cfg[0].tid = 0;
	cfg[0].exit = 0;
	
	for ( unsigned int idx = 0; idx < sizeof( cfg ) / sizeof( cfg[0] ); idx++ )
	{
		pthread_attr_t		attr;

		pthread_attr_init( &attr );
		pthread_create( &cfg[idx].tid, &attr, write_to_file, ( void * ) &cfg[idx] );
	}

	start_cli( cfg, sizeof( cfg ) / sizeof( cfg[0] ) );
	while( 1 ) { sleep( 5 ); }
	return 0;
}


