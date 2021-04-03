// 下位
//
//
//

#include <math.h>
#ifdef _WIN32
#include <conio.h>
#endif
#include <stdlib.h>
#include "pulmem.h"
#include "pulbrd.h"


#include "stdio.h"
#include "inoutdev.h"



void PULBRD::outpulserb( int off, int dat )
{
	outdevb( base_address+(unsigned)off, (unsigned)dat );

}

void PULBRD::outburst_start()
{
	OutBurstStart();
}

void PULBRD::outburst_flush()
{
	OutBurstFlush();
}

void PULBRD::outpulserw( int off, unsigned int dat )
{
	outdevw( base_address+(unsigned)off, (unsigned)dat );
}

int PULBRD::inpulserb( int off )
{
	return indevb( base_address+ off );
}


int PULBRD::status()
{
	return inpulserb(3);
}

void PULBRD::WRamAdd(long a)
{
	outpulserw( 0, a & 0xffffl);
	outpulserb( 2, a >> 16 );
}

void PULBRD::ResetStatus()
{
	outpulserb( 3, 0 );
}

void PULBRD::addinc()
{
	outpulserb( 13, 2);
}

void PULBRD::WLoopCounter( uint16_t n)
{
	outpulserw( 10, n );
}


void PULBRD::WData( uint32_t dat)
{
	outpulserw( 8,  dat &  0xffff );//bit0..15
	outpulserw( 14, dat >> 16     );//bit16..31
}


void PULBRD::WTime( long t)
{
	outpulserw( 4, 0xffffL & t );	//下位１６ビット
	outpulserw( 6,     t >> 16 );	//上位１６ビット
}


void PULBRD::writeData( PULDATU tab[], int nums)//FIX20150205
{
	int i;

	outburst_start();
//	printf("Transfer ST:");
	tab[nums-1].t = 0xffffffffUL;
	WRamAdd( 0 );
	for(i = 0 ; tab[i].t != 0xffffffffUL; i++){
		WTime( tab[i].t );
		WData( tab[i].d[0] ); 		// data width 32bits
		addinc();
	}
//	printf("ED\n");
	outburst_flush();

}

int PULBRD::isRun()
{
	if( disable){
//		return rand()/32700;
		return 0;
	}
	if( 4 & status()){
		ResetStatus();
		return 1;
	}
	ResetStatus();
	return 0;
}

PULBRD::PULBRD()
{
	base_address = 0x00;
	disable = 0;
}

PULBRD::PULBRD(int ad)
{
	base_address = ad;
	disable = 0;
}

void PULBRD::start( long startadd=0 )
{
	stop(0);
	WRamAdd(startadd);
	if(extClockEnable){
		outpulserb( 12,0x0c );	// fix 030804
	} else {
		outpulserb( 12,0x08 );	// fix 030804
	}
	outpulserb( 13,1);
}

void PULBRD::stopNow( uint32_t dat=0 )
{
	outpulserb( 13, 0 );
	outpulserb( 12, 2 );
	outpulserw( 8, dat&0xffff );
	outpulserw( 14,dat>>16    );
	outpulserb( 12, 0 );
}

void PULBRD::stop( uint32_t dat=0 )
{
	if( waiting ){
		if( isRun()){
			outpulserb( 13, 5 );
			outpulserb( 13, 5 );
			while( isRun());
		}
	}
	stopNow(dat);
}

/*** EOF ***/
