#ifndef _PULBRD_H_
//
//
//
//
//

#include "pulmem.h"

class PULBRD {
private:
	void WTime( long t);
	void WData( uint32_t dat);
	void WRamAdd(long a);
	void ResetStatus();
	void addinc();
	int status();
	void outpulserb( int off, int dat );
	void outpulserw( int off, unsigned int dat );
	int inpulserb( int off);
public:
	int base_address;//if USB then fix:0 else BOARD IO PORT ADDRESS
	void outburst_start();
	void outburst_flush();
	void stopNow( uint32_t dat );
	void WLoopCounter( uint16_t n);
	int disable;
	int waiting;	// くり返しは、待ち合わせる
	int	rsv1; //(add)
	int	rsv2; //
	int extClockEnable;
	int isRun();
	void start( long startadd );
	void stop( uint32_t dat );
	void writeData( PULDATU tab[], int nums );//FIX20150205
	PULBRD( int add );	//for ISA board
	PULBRD();			//for USB board
};


#define _PULBRD_H_
#endif

/*** EOF ***/
