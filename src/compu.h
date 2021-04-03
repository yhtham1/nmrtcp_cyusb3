//
// N210-1026S,T GPIB interpreter
//

#define _POSIX_SOURCE  1   /* POSIX comliant source (POSIX)*/

#include <lua.hpp>

#include "pulmem.h"
#include "pulbrd.h"
#include "pulgen.h"
#include "etools.h"
#include "pulser.h"
#include "unistd.h"
#include <stdint.h>
#include <ctype.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#include "strtok_r.h"
#include <string.h>
#include "etools.h"
#include "moddv14.h"
#include "inoutdev.h"
#include "tcp.h"

//#define dprintf	printf


extern char *IDN_MSG_PULSER;
extern char *IDN_MSG_AD;
extern char *IDN_MSG_RF;
extern PULGEN pul1;
extern int HideDisplay;	// 000719


void trimString( char *str );
int cutTimes( char *buf, int *ch, double *t1, double *t2 );
int cutParams( char *buf, double *t1, uint32_t *a, uint16_t *b );
int cutThreeData( char *buf, uint32_t *p1, uint32_t *p2, uint32_t *p3 );
int cutFourData( char *buf, uint32_t *p1, uint32_t *p2, uint32_t *p3, int32_t *p4 );

char* checkword( char *buf, char const *word, int *u );
int getkeyword( char *buf, const char *keyword, char *ans );

void ans_fd( int fd, const char *obuf, char **ans );
int compuPULSER(char *buf, PULGEN& p, int fd, char **ans);
int compuAD(char *buf, int fd, char **ans);
int compuRF(char *buf, int fd, char **ans);

/*** EOF ***/
