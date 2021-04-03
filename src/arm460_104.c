//////////////////////////////////////////
// armadillo-460用 ＰＣ１０４バス関数
//////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "types.h"
#include "inoutdev.h"

volatile uchar *BUS104_IO;
#define PC104_ADD		(0x0300)		//IF125のDIPSW

#define ADRS_REG	(PC104_ADD+2)
#define DATA_REG	(PC104_ADD)


int inb104(int add)
{
	int ans;
	ans = *(volatile uint8_t *)(BUS104_IO+add);
	return ans;
}

void outb104(int add, unsigned dat)
{
	*(volatile uint8_t *)(BUS104_IO+add) = (uchar)dat;
}


int init104(void)
{
	int fd;
	fd = open("/dev/mem",O_RDWR|O_SYNC);
	if(fd < 0) { fprintf(stderr,"cannot open /dev/mem\n"); return 1; }
	BUS104_IO = (volatile uchar*)mmap(0, 0x10000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0xb2000000);
	close(fd);
	if(BUS104_IO == MAP_FAILED) { fprintf(stderr,"cannot BUS104_IO\n"); return 1; }
	return 0;
}


void close104(void)
{
	munmap( (void*)BUS104_IO, 0x10000 );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int indevb(unsigned add )
{
	outb104( ADRS_REG, add );
	return inb104(DATA_REG);
}
int indevw(unsigned add )
{
	int L, H;
	L = inb104(add);
	H = inb104(add);
	return ((H&0xff)<<8) | (L&0xff);
}

void outdevb( int add, int dat )
{
	outb104( PC104_ADD+2, (uchar)add );
	outb104( PC104_ADD+0, (uchar)dat );
}
void outdevb( unsigned add, unsigned dat )
{
	outb104( ADRS_REG, add );
	outb104( DATA_REG, dat &  0xff );
}
void outdevw( unsigned add, unsigned dat )
{
	outdevb( add+0, (dat &  0xff) );
	outdevb( add+1, (dat >> 0x08) );
}

void OutBurstStart()
{
}

void OutBurstFlush()
{
}

void InBurstB(unsigned port, uint8_t *ans, int nbyte)
{
}




void open_io_dev()
{
	init104();
}

void close_io_dev()
{
	close104();
}



/*** EOF ***/
