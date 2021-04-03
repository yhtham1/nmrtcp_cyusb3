//ＵＴＦ８
#ifndef _INOUTDEV_H_
#define _INOUTDEV_H_
#include <stdint.h>

void open_io_dev();
void close_io_dev();

void outdevb( unsigned add, unsigned dat );
void outdevw( unsigned add, unsigned dat );

int indevb( unsigned add );
int indevw( unsigned add );

void InBurstB(unsigned port, uint8_t *ans, int nbyte);
void OutBurstStart();
void OutBurstFlush();


/*** EOF ***/
#endif
