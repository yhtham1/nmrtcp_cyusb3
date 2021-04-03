//CGIを使用するための関数
#ifndef _CGIFUNC_H_
#define _CGIFUNC_H_


#include <stdio.h>
#include "stdlib.h"
#include "string.h"
#include "unistd.h"



int ReadPOST( char **argans );
void HTTP_header( char *p, char *p2 );
void HTTP_footer( void );
int get_pars_str( char *db, int dblen, const char *keyword, char *a );
void get_pars_long( char *db, int dblen, const char *keyword, long *a, long def );
void get_pars_double( char *db, int dblen, const char *keyword, double *a, double def );
void get_pars_ip( char *db, int dblen, char *keyword, unsigned long *a );
void base64decode(char *src, char **ans_o, int *len);
void disparg(char *tt, int clen);


#endif
/*** EOF ***/
