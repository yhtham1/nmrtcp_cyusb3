//
//
//   改版履歴  FIX20011029:QPSK2ndを0.5u先行するように変更。
//             000530:t2とエコーの関係を変えました。
//             001023:時間ブロック算出計算をFIX
//
#ifdef _WIN32
#include <conio.h>
#endif

#include "pulmem.h"
#include "pulbrd.h"
#include "pulgen.h"
#include "etools.h"
#include <memory.h>
#include <stdio.h>
//#define opt.QPSKDELAY	(1.5e-6) to PULGEN::opt.opt.QPSKDELAY




void PULGEN::init()
{
	memset( &opt, '\0', sizeof(Options));
	opt.SENKOU_LEN = 10e-6;	//tx2predelay
	opt.KOUKOU_LEN = 0.0;	//tx2postdelay
	opt.QPSKDELAY = 1.5e-6; //hardware depend QPSK swiching delay
	isDouble = 0;
	blank = 0.0;
	cpn = 0;
	cpw = 0.0;
	cpi = 0.0;
	fpw = 0.0;
	spw = 0.0;
	tj = 0.0;
	t2 = 0.0;
	ADOFF = 0.0;
	OSCILLO = 0;
	PowLevel = 0;
	QPSKComb = 0;
	QPSK1st = 0;
	QPSK2nd = 0;
	ADTrigLocation = 0;
	EXTTRIG = 0;
	BlankIsLoopTime = 0;
	UseComb = 0;
}

void PULGEN::clear(double t)
{
	double t2;
	int bk;
	t2 = 0xfefffffful / mem.pulserFreq; // 1メモリ当たりの時間
	bk = (int)(t / t2);
	bk += 1;
	if( MAX_PULMEM < bk ){
		printf("PULGEN::clear():memory over flow %d\n", bk);
	}
	mem.clr(bk);// 30x427Sec
}
void PULGEN::clear()
{
	clear( 90000.0 );//FIX20150205 838cells@40MHz
}

PULGEN::PULGEN( int add, void *buf )
{
	init();
	mem.databuf = (PULDATU *)buf;
	brd.base_address = add;
//	clear();
}

void PULGEN::dispmem(int fd, int stpt, int lines)
{
	int fd_printf(int fd, const char *fmt, ...);
	int i, cd;
	uint32_t t, d;
	double tt, f;
	unsigned long int tL, dL;
	f = mem.pulserFreq;

	if( mem.u_max < stpt+lines ){
		fd_printf(fd, "target area is too large\r\nEND\r\n");
		return;
	}

	fd_printf(fd, "clock=%.lfHz\r\n", f );
	fd_printf(fd, "  addr:timedata:bitdata :duration\r\n");
	for( i = stpt ; i < stpt+lines ; i++){
		t = mem.databuf[i].t;
		d = mem.databuf[i].d[0];
		tL = t;
		dL = d;
		if( (0xff000000 & t)==0xff000000){
			cd = 0xff & (t >> 16);
			switch( cd ){
			case 0xff: fd_printf(fd, "%6d:%08lX:%08lX:END OF MEMORY\r\n", i,tL,dL ); break;
			case 0x80: fd_printf(fd, "%6d:%08lX:%08lX:TRG WAITING\r\n", i,tL,dL ); break;
			case 0x40: fd_printf(fd, "%6d:%08lX:%08lX:STOP\r\n", i,tL,dL ); break;
			case 0x20: fd_printf(fd, "%6d:%08lX:%08lX:GOTO %04lu\r\n", i, tL, dL,t & 0xfffful); break;
			}
		} else {
			char buf[20];
			tt = (double)t/f;
			TimeToStr( buf, tt );
			fd_printf(fd, "%6d:%08lX:%08lX:%s\r\n", i, tL, dL, buf );
		}
	}
	fd_printf(fd, "END\r\n");
}

void PULGEN::Make1Bit( double st, double l, uint32_t mask, uint32_t pat)
{
	double ed;
	ed = st + l;
	mem.lvl( st, ed, mask, pat );
}

void PULGEN::MakeCmd( double t, uint32_t patt)
{
	mem.com( t, patt, 0 );
}

void PULGEN::MakeCmd( double t, uint32_t patt, uint16_t patt2)
{
	mem.com( t, patt, patt2 );
}

void PULGEN::MakeLoop( double t, double a)
{
	MakeCmd( t, 0xff200000ul );
}

void PULGEN::MakeBlank(double st, double l)
{
	Make1Bit( st, l, ~1, 0 );
}

void PULGEN::MakeTXGate(double st, double l)
{
	if( l == 0.0 ) return;
	if( opt.SENKOU_LEN <= st ) MakeTX2( st - opt.SENKOU_LEN, l + opt.SENKOU_LEN + opt.KOUKOU_LEN );
	if( opt.AUX1_FLEN <= st ) MakeAUX1( st - opt.AUX1_FLEN, l + opt.AUX1_FLEN + opt.AUX1_BLEN );
	Make1Bit( st, l, ~1, 1 );
}

void PULGEN::MakeAUX3( double st, double l )
{
	Make1Bit( st, l, ~0x800, 0x800 );
}


void PULGEN::MakeEXTTrig( double st )
{
	if( st == 0.0 ){
		st = st + 0.3e-6;
	}
	Make1Bit( st, 1e-6, ~0,0 );
	mem.com( st, 0xff800000, 0 );
}

void PULGEN::MakeOSTrig( double st )
{
	Make1Bit( st, 1e-6, ~0x40 , 0x40 );
}

void PULGEN::MakeQPSK( double st, double l, int phase )
{
	int pat;
	pat = (phase & 3) * 8;
	Make1Bit( st, l, ~0x18 , pat );
}


void PULGEN::MakeTX2( double st, double l )
{
	Make1Bit( st, l, ~0x2 , 0x2 );
}

void PULGEN::MakeAUX1( double st, double l )
{
	Make1Bit( st, l, ~0x4 , 0x4 );
}

void PULGEN::MakeAUX2( double st, double l )
{
	Make1Bit( st, l, ~0x20 , 0x20 );
}

void PULGEN::MakePowLevel( double st, double l, int pw )
{
	st = l;
	// 京産大 only
}

void PULGEN::MakeAUX4( double st, double l )
{
	Make1Bit( st, l, ~0x4000 , 0x4000 );
}

void PULGEN::Make1st( double st, double l )
{
	if( l == 0.0 ) return;
	Make1Bit( st, l, ~0x1000 , 0x1000 );
}

void PULGEN::MakeMeter( double st )
{
	Make1Bit( st, 1e-3, ~0x80 , 0x80 );
}

void PULGEN::Make2nd( double st, double l )
{
	if( l == 0.0 ) return;
	Make1Bit( st, l, ~0x2000 , 0x2000 );
}

void PULGEN::MakeComb( double st, double l )
{
	if( l == 0.0 ) return;
	Make1Bit( st, l, ~0x800 , 0x800 );
}

void PULGEN::MakePGON( double st, double l )
{
	Make1Bit( st, l, ~0x100 , 0x100 );
}

void PULGEN::MakeADTrig( double st )
{
	Make1Bit( st, 1e-6, ~0x200 , 0x200 );
}

void PULGEN::MakeEnd( double t )
{
	MakeCmd( t, 0xff400000 );
}

int PULGEN::check()
{
	error = 0;
	
	return error;
}

double PULGEN::MakeOneT1T2( double tSt )
{
	int i;
	double t, tx2t, tt;
//	if( (tSt + ADOFF ) < 0.0 ) tSt -= ADOFF;//FIX20181226失敗
	t = tSt + opt.SENKOU_LEN;
	if( EXTTRIG) MakeEXTTrig(t);

	//******************************* COMB pulse generate
	if( OSCILLO == 0) MakeOSTrig(t);		//TRG.OUT is start of COMB pulse
	if( 0 != UseComb && cpn ){
		tx2t = t;
		for( i = 0 ; i < cpn ; i++){
			MakeTXGate( t, cpw );
			MakeComb( t, cpw );
			t += cpw + cpi;
		}
		t -= cpi;
		t += tj;
		MakeQPSK( tx2t - opt.QPSKDELAY, t-tx2t-cpi + opt.QPSKDELAY,QPSKComb); //FIX20011029
	}
	if( OSCILLO == 1) MakeOSTrig( t-tj );//TRG.OUT is end of COMB pulse
	if( OSCILLO == 2) MakeOSTrig( t    );//TRG.OUT is 1st pulse
	//******************************* 1ST pulse generate
	MakeQPSK( t-opt.QPSKDELAY, fpw + opt.QPSKDELAY, QPSK1st );//FIX20011029
	MakeTXGate( t, fpw );
	Make1st( t, fpw );
	t += fpw;
	MakeMeter( t );
	if( opt.First_AD) MakeADTrig( t + ADOFF );
	if( 0 == isDouble ){
		MakeADTrig( t + ADOFF );
		if( OSCILLO==4) MakeOSTrig( t - fpw + t2);	//TRG.OUT is AD.TRG
	}
	if( OSCILLO == 3 ) MakeOSTrig( t - fpw + t2 );	//TRG.OUT is 2nd pulse
	//******************************* 2ND pulse generate
	if( isDouble ){
#if 1		//TAU position SELection
		t = t - fpw + t2;	// edge to edge
#else
		t = t + t2;			// center to center
#endif
		MakeQPSK( t - opt.QPSKDELAY, spw + opt.QPSKDELAY, QPSK2nd );	//FIX20011029
		MakeTXGate( t, spw );
		Make2nd( t, spw );
		if( ADTrigLocation == 0 ){
			tt = t + spw + ADOFF + t2;	//trigger is SPIN ECHO position
		} else {
			tt = t + ADOFF + spw ;		//trigger is FID       position
		};
		MakeADTrig( tt );
		if( OSCILLO == 4 ) MakeOSTrig( tt );	//TRG.OUT is AD.TRG
		t += spw;
	}
	//******************************* wait delay generate
//	MakePowLevel( tSt, t - tSt, PowLevel );	/* for 京産大 */
//	MakeAUX4( tSt, t - tSt + 50e-6 );	/* for 東大物性研 */
	if( BlankIsLoopTime ){
		MakeBlank( t, blank );	// blank is repeat   time
		t = tSt + blank;
	} else {
		MakeBlank( t, blank );	// blank is interval time
		t += blank;
	}
	return t;
}


void PULGEN::make()
{
	double t;
//	t = fpw + spw + blank + (cpw + cpi )*cpn;
	t = fpw + spw + blank + t2 + tj + blank + (cpw + cpi )*cpn; // FIX 001023
	clear(t+10.0);

	t = 0.0;	//FIX20150526
//FIX20150526	t = opt.SENKOU_LEN;//先行なし
	t = MakeOneT1T2( t );
	MakeLoop( t, 0.0 );
	MakeEnd( t+1e-6 );
}

double PULGEN::duty()
{
	double t1, t2, t;
	t1 = pulselen();
	t2 = blank;
	if( UseComb ) t2 += tj+(cpi * (double)cpn);
	if( BlankIsLoopTime ) t2 -= t1;
	if( t2 <= 0.0) return 1.0;
	t = t1/(t1+t2);
	return t;
}

double PULGEN::pulselen()
{
	double t;
	t = fpw;
	if( UseComb ) t += (cpw * (double)cpn);
	if( isDouble ) t += spw;
	return t;
}


void PULGEN::mem2brd()
{
//	printf("mem2brd:%d\n", mem.u_max );
	brd.writeData( mem.databuf, mem.u_max );
}

void PULGEN::start(unsigned int num)
{
	if( num != 0 ) loopn = num;
	brd.stop(0);
//	printf("pulgen::start %d\n", loopn );
	brd.WLoopCounter( loopn );
	brd.start(0l);
}

void PULGEN::stop()
{
	brd.stop(0l);
}

/*** EOF ***/
