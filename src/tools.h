//
// LINUX共通関数
//

#ifndef _TOOLS_H_
#define _TOOLS_H_

#include "fcntl.h"
#include "unistd.h"

void dprintf(const char* fmt, ...);
extern int debugMessages;
unsigned long crc32(void *dat, int len);

int SaveBlock( char *fn, void *buf, int size );
int LoadBlock( char *fn, unsigned char *buf, int size );




#endif
