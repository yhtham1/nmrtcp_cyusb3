//////////////////////////////////////////
// LINUX共通関数
//////////////////////////////////////////


#include "tools.h"

#include <stdarg.h>
#include <stdio.h>

int debugMessages=0;

void dprintf(const char* fmt, ...) // DEBUG printf DIPSW MASKED
{
	va_list  argptr;
	char str[100];
	if( debugMessages ){
		va_start( argptr, fmt );
		vsnprintf( str, sizeof str, fmt, argptr );//FIX20201115
		printf(str);
		va_end( argptr );
	}
}

int SaveBlock( char *fn, void *buf, int size )
{
//	char str[80];
	int fd;
	fd = open( fn, O_WRONLY );
	
	if( fd == -1 ){
//		sprintf(str,"open error[%s]\r", fn );
//		dprintf(str);
		return -1;
	}
	write( fd, (char *)buf, size );
	close( fd );
	return 0;
}

int LoadBlock( char *fn, unsigned char *buf, int size )
{
	int ans, fd;
	fd = open( fn, O_RDONLY );

	
	if( fd == -1 ){
		return -1;
	}
	ans = read( fd, (char *)buf, size );
	close( fd );
	return ans;
}


/*** EOF ***/
