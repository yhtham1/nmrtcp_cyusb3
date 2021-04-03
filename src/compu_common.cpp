//
// N210-1026S,T GPIB interpreter
//

#define _POSIX_SOURCE  1   /* POSIX comliant source (POSIX)*/
#include "compu.h"
#include <stdarg.h>


void trimString( char *str )
{
	int n;
	// trimming head
	while( *str == ' ' ){
		n = strlen(str);
		memmove(str, &str[1], n-1);
		str[n-1] = '\0';
	}

	//trimming tail
	for( n=strlen(str); str[n-1] == ' ' ; n = strlen(str) ){
		str[n-1] = '\0';
	}

}




int cutTimes( char *buf, int *ch, double *t1, double *t2 )
{
	char *p, *saveptr;
	int ans;
	*t1 = 0.0;	*t2 = 0.0;
	ans = 0;
	p = MY_strtok_r( buf, ",", &saveptr );
	if( p ){ *ch = (int)strtol(p, NULL, 10); ans++; }
	
	p = MY_strtok_r( NULL, ",", &saveptr );
	if( p ){ *t1 = StrToTime( p ); ans++; }

	p = MY_strtok_r( NULL, ",", &saveptr );
	if( p ){ *t2 = StrToTime( p ); ans++; }
	return ans;
}

int cutParams( char *buf, double *t1, uint32_t *a, uint16_t *b )
{
	char *p, *saveptr;
	int ans;
	*t1 = 0.0;
	ans = 0;
	p = MY_strtok_r( buf, ",", &saveptr );
	if( p ){ *t1 = StrToTime( p ); ans++; }

	p = MY_strtok_r( NULL, ",", &saveptr );
	if( p ){ *a = strtoul(p, NULL, 0); ans++; }

	p = MY_strtok_r( NULL, ",", &saveptr );
	if( p ){ *b = (uint16_t)strtol(p, NULL, 0); ans++; }
	return ans;
}


int cutThreeData( char *buf, uint32_t *p1, uint32_t *p2, uint32_t *p3 )
{
	char *p;
	char *saveptr;
	int ans;
	*p1 = 0;
	ans = 0;
	p = MY_strtok_r( buf, ",",  &saveptr);
	if( p ){ *p1 = strtoul( p, NULL, 0); ans++; } else { return ans; }

	p = MY_strtok_r( NULL, ",", &saveptr );
	if( p ){ *p2 = strtoul( p, NULL, 0); ans++; } else { return ans; }

	p = MY_strtok_r( NULL, ",", &saveptr );
	if( p ){ *p3 = strtoul( p, NULL, 0); ans++; }
	return ans;
}


int cutFourData( char *buf, uint32_t *p1, uint32_t *p2, uint32_t *p3, int32_t *p4 )
{
	char *p;
	char *saveptr;
	int ans;
	*p1 = 0;
	ans = 0;
	p = MY_strtok_r( buf, ",",  &saveptr);
	if( p ){ *p1 = strtoul( p, NULL, 0); ans++; } else { return ans; }

	p = MY_strtok_r( NULL, ",", &saveptr );
	if( p ){ *p2 = strtoul( p, NULL, 0); ans++; } else { return ans; }

	p = MY_strtok_r( NULL, ",", &saveptr );
	if( p ){ *p3 = strtoul( p, NULL, 0); ans++; } else { return ans; }

	p = MY_strtok_r( NULL, ",", &saveptr );
	if( p ){ *p4 = strtol( p, NULL, 0); ans++; }

	return ans;
}

char* checkword( char *buf, char const *word, int *u )
{
	int a;
	a = strlen(word);
	if( 0 == memcmp( buf, word, a )){
		(*u) |= 1;
		return &buf[a];	//一致
	}
	return NULL;		//不一致
}


//int checkword( char *buf, char const *word, int *u )
//{
//	if( 0 == memcmp( buf, word, strlen(word))){
//		(*u) |= 1;
//		return 1;
//	}
//	return 0;
//}


int getkeyword( char *buf, const char *keyword, char *ans )
{
	int nk;
	char *ptst, *pted, *apt, *ptwd;
	nk = strlen(keyword);
	ptst = strstr( buf, keyword );
	if( NULL == ptst ) return -1;
	pted = strstr( &ptst[nk], "," );
	if( NULL == pted ){
		printf("internal error\n");
		return -1;
	}
	apt = ans;
	ptwd = &ptst[nk];
	while( ',' != *ptwd ){
		*apt = *ptwd;
		apt++;
		ptwd++;
	}
	*apt = '\0';


	return 0;
}


int fd_printf(int fd, const char *fmt, ...)
{
	int num;
	int ans;//FIX20201115
	va_list  argptr;
	char str[100];
	va_start( argptr, fmt );
	ans = vsnprintf( str, sizeof str,fmt, argptr );//FIX20201115
	if( fd == 0 ){
		printf(str);
	} else {
		num = strlen(str);
#ifdef _WIN32
		send( fd, str, num,0 );
#else
		write( fd, str, num );
#endif
	}
	va_end( argptr );
	return ans;//FIX20201115
}


//void ans_fd( int fd, const char *obuf, char **ans )
//{
//	int num;
//	char *buf;
//
//	num = strlen(obuf);
//
//	if( fd == 0 ){ // output is Lua
//		if( NULL != ans ){
//			*ans = (char *)realloc( *ans, num+3 );//FIX20150603
//			if( NULL == *ans ){ perror("out of memory"); exit(1); }
//			strcpy( *ans, obuf );
//			strcat(*ans, "\r\n");
//		}
//		return ;
//	}
//#ifdef _WIN32
//	send( fd, obuf, num,0 );
//	send( fd, "\r\n", 2,0 );
//#else
//	write( fd, obuf, num );
//	write( fd, "\r\n", 2 );
//#endif
//}



void ans_fd( int fd, const char *obuf, char **ans )
{
	int num;
	char *buf;

	num = strlen(obuf);

	if( fd == 0 ){
		if( NULL != ans ){
			*ans = (char *)realloc( *ans, num+3 );//FIX20150603
			if( NULL == *ans ){ perror("out of memory"); exit(1); }
			strcpy( *ans, obuf );
			strcat(*ans, "\r\n");
		}
		return ;
	}
	buf = (char *)malloc(num+3);
	if( NULL == buf ){ perror("out of memory"); exit(1); }
	strcpy( buf, obuf );
	strcat( buf, "\r\n");
#ifdef _WIN32
	send( fd, buf, num+2,0 );
#else
	write( fd, buf, num+2 );
#endif
	free(buf);
}



/*** EOF ***/
