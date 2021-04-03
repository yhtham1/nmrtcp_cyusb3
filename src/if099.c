//
///////////////////////////////////////////////////////////
// IF099基板用    IF099基板を用いてUSBバスを読み書き
// USB read/write functions for IF099 board
///////////////////////////////////////////////////////////


// 修正履歴
//
//
//
#if 0
	Hierarchy of functions

	outpulserw()
		outdevw()
			uio_outw()

#endif

#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include "inoutdev.h"


int uio_inb(int add);
int uio_inw(int add);
void uio_outb(int add, int dat);
void uio_outw(int add, int dat);
void uio_outBurstStart();
void uio_outBurstFlush();
void uio_close(void);
void uio_inburst(int portadd, uint8_t *ans, int nbyte);

void OutBurstStart()
{
	uio_outBurstStart();
}

void OutBurstFlush()
{
	uio_outBurstFlush();
}


void outdevb( unsigned add, unsigned dat )
{
	uio_outb( add, dat );
}

void outdevw( unsigned add, unsigned dat )
{
	uio_outw( add, dat );
}

int indevb( unsigned add )
{
	return uio_inb(add);
}
int indevw( unsigned add )
{
	return uio_inw(add);
}

void InBurstB(unsigned port, uint8_t *ans, int nbyte)
{
	uio_inburst(port, ans, nbyte);
}

void open_io_dev()
{
	void uio_init(const char *wave_filename);
	uio_init("slow_dat.bin");
}


void close_io_dev()
{
	uio_close();
}


/*** EOF ***/
