#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "types.h"
#include "etools.h"

#ifdef _WIN32
#else
#include <unistd.h>

char tolower( char c )
{
	if( 'A' <= c && c <= 'Z' ){
		return (c + 0x20);
	}
	return c;
}

char toupper( char c )
{
	if( (uchar)'a' <= (uchar)c && (uchar)c <= (uchar)'z' ){
		return (c - 0x20);
	}
	return c;
}


char* strupr( char *str)
{
	char *p;
	p = str;
	while (*p){
        *p = toupper (*p) ;
        p++;
    }
    return str;
}


char* strlwr(char *str)
{
	char *p;
	p = str;
	while (*p){
        *p = tolower(*p) ;
        p++;
    }
    return str;
}

void Sleep(int t)
{
	usleep(t*1000);
}



#endif



double StrToTime(char *str)
{
	char buf[100];
	double t;
	strcpy( buf, str );
	strupr( buf );
	if( 1 != sscanf( buf, "%lf", &t )) return 0.0;
	if( NULL != strchr( buf, 'U' )){
		t = t * 1e-6;
		return t;
	}
	if( NULL != strchr( buf, 'M' )){
		t = t * 1e-3;
		return t;
	}
	if( NULL != strchr( buf, 'S' )){
		return t;
	}
	return t;
}

void IntToStr( char *buf, int d )
{
	sprintf( buf, "%d", d );
	return;
}

void TimeToStrE( char *buf, double t )
{
	sprintf( buf, "%.16g", t );
	return;
}

void TimeToStr( char *buf, double t )
{
	if( 9990 < fabs( t )){ sprintf( buf, "%.0lfS", t ); return; }
	if( 999 < fabs( t )){ sprintf( buf, "%.0lfS", t ); return; }
	if( 99.9 < fabs( t )){ sprintf( buf, "%.1lfS", t ); return; }
	if( 9.99 < fabs( t )){ sprintf( buf, "%.2lfS", t ); return; }
	if( 0.999 < fabs( t )){ sprintf( buf, "%.3lfS", t ); return; }
	if( 99.9e-3 < fabs( t )){ sprintf( buf, "%.0lfmS", t * 1e3 ); return; }
	if( 9.99e-3 < fabs( t )){ sprintf( buf, "%.1lfmS", t * 1e3 ); return; }
	if( 0.999e-3 < fabs( t )){ sprintf( buf, "%.2lfmS", t * 1e3 ); return; }
	if( 99.9e-6 <= fabs( t )){ sprintf( buf, "%.0lfuS", t * 1e6 ); return; }
	sprintf( buf, "%.1lfuS", t * 1e6 ); return;
}

double StrToHz(char *str)
{
	char buf[100];
	double t;
	strcpy( buf, str );
	strupr( buf );
	if( 1 != sscanf( buf, "%lf", &t )) return 0.0;
	if( NULL != strchr( buf, 'K' )){
		t = t * 1e3;
		return t;
	}
	if( NULL != strchr( buf, 'M' )){
		t = t * 1e6;
		return t;
	}
	if( NULL != strchr( buf, 'G' )){
		t = t * 1e9;
		return t;
	}
	return t;
}

void HzToStr( char *buf, double t )
{
	if( 999e6 < fabs( t )){ sprintf( buf, "%.3lfGHz", t * 1e-9 ); return; }
	if( 99.9e6 < fabs( t )){ sprintf( buf, "%.1lfMHz", t * 1e-6 ); return; }
	if( 9.99e6 < fabs( t )){ sprintf( buf, "%.2lfMHz", t *1e-6 ); return; }
	if( 0.999e6 < fabs( t )){ sprintf( buf, "%.3lfMHz", t *1e-6 ); return; }
	if( 99.9e3 < fabs( t )){ sprintf( buf, "%.1lfkHz", t * 1e-3 ); return; }
	if( 9.99e3 < fabs( t )){ sprintf( buf, "%.2lfkHz", t * 1e-3 ); return; }
	if( 0.999e3 < fabs( t )){ sprintf( buf, "%.3lfkHz", t * 1e-3 ); return; }
	sprintf( buf, "%.1lfHz", t  ); return;
}


#if 0
void main()
{
	double t;
	char buf[200];
	for(;;){
		gets( buf );
		if( 0==strcmp( buf, "" )){
			return ;
		}
		t = StrToTime( buf );
		TimeToStr( buf, t );
		printf( "%s\n", buf );
	}
}
#endif


/*** EOF ***/


