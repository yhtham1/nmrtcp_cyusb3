#ifndef _PULMEM_H_

//
//
//
//
//
#include <stdint.h>

#define MAX_PULMEM (3000)	//FIX20150205

//typedef unsigned long uint32_t;
//typedef unsigned short uint16_t;

#include "llong.h"

#define PULDAT_DL	(3)

class PULDATU {
public:
	uint32_t t;
	uint32_t d[PULDAT_DL];				//FIX20150205 FIX20150513
};

//typedef LPSAFEARRAY FAR *HAD;

class PULDATC {
private:
	PULDATU*	datp;
	PULDATU*	dat( uint32_t pt );
	uint32_t 	t( uint32_t dt );
	uint32_t 	d32( uint32_t pt );
	uint32_t 	dt( llong tim, uint32_t& diff );
	void 		ins( uint32_t pt );
	void 		del( uint32_t dt );
	uint32_t 	end( uint32_t pt );
	uint32_t 	edg( llong tim );
	void 		lvl( uint32_t,uint32_t,uint32_t,uint32_t, uint32_t mask, uint32_t pat);
	void 		lvl( llong sttim, llong edtim, uint32_t mask, uint32_t pat);
	void 		seclong(llong&,long,long);
	void		dbllong( llong &ans, double t );
	void		com( llong tim, uint32_t t, uint16_t d );

public:
	void 		clearpulm();
	short 		base;
//	PULDATU 	databuf[1000];	stack overflow
	PULDATU*	databuf;//data area
	~PULDATC();
	PULDATC();
	PULDATC(PULDATU *p);
	void		clr( uint16_t leng );
	void 		com( long sec, long usec, uint32_t t, uint16_t d );
	void 		com( double s, uint32_t t, uint16_t d);
	void		lvl( double st, double ed, uint32_t mask, uint32_t pat);
	long		pulserFreq;		//pulser Clock Frequency(Hz)
	int 		u_max;			//number of databuf[]
};

#define _PULMEM_H_
#endif

/*** EOF ***/

