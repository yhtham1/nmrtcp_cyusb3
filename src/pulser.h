#ifndef _PULSER_H_
#define _PULSER_H_
#include "etools.h"

enum SystemColor {
	cBACK1,
	cTEXT,
	cGETFOCUS,
	cLOSTFOCUS,
	cINPUT,
	cWAKUBACK,
	cWAKU,
	cSIGNAL,
	cTLK,
	cLSN,
	cSRQ,
	cFUNC1,
	cFUNC2,
	cCOMB,
	c2ND,
	cRF
//	cCAPITAL
};



void InitMouse();
void MouseOn();
void MouseOff();
//void AddSpot(P2D p1, P2D p2, int id);
int SearchSpot(int x, int y );
int isMouse(int *x, int *y);
void charrectangle( int x1, int y1, int x2, int y2 );
void charrectangle2( int x1, int y1, int x2, int y2, int r );
void charbar( int x1, int y1, int x2, int y2 );


#define VERSION	"2.00"
// rev 2.00 for minGW
// rev 1.70 USB PULSER( IF125+PG021A)
// rev 1.62 pulboard.cpp メモリーサイズを拡大 main MEMSIZE nnnn 060627
// rev 1.61b pulboard.cpp 新型ボードに対応(PULBRD::start()) 030804
// rev 1.61a ibslave.cpp add 'transfer' fix2 for HANDAI 030727
// rev 1.61 ibslave.cpp add 'transfer' fix2
// rev 1.60 ibslave.cpp add 'transfer' fix
// rev 1.52 pulgen.cpp add 'QPSK timming' fix
// rev 1.51 compu.cpp add 'writeram' fix 
// rev 1.50 compu.cpp add 'writeram' command 
// rev 1.42 compu.cpp 'erasepulse' command FIX
// rev 1.41 pulgen.cpp bug fix 001023
// rev 1.40 add hide
// rev 1.30 add makecommand, makepulse ChipIFC()
// rev 1.22 bugfix


#endif
/*** EOF ***/
