#ifndef _PULGEN_H_


#include "pulbrd.h"

typedef struct {
	int ResetAtOpen;	//リセットする
	int StopAtClose;	//終了時に、ボードを止める
	double SENKOU_LEN;	//tx2 predelay
	double KOUKOU_LEN;	//tx2 postdelay 後行パルスの長さ
	double QPSKDELAY;
	double AUX1_FLEN;	//ＡＵＸ１の先行パルス長さ
	double AUX1_BLEN;	//ＡＵＸ２の後行パルスの長さ
	int First_AD;		//
	int CheckPara;		// パラメータのチェックを行う。
	int Duty;			// MaxDuty(%) 0 == nocheck;
	double PulseLen;	// MAX PulseLen 0 == nocheck;
	unsigned puladd;		//
	unsigned usbadd;		//
	unsigned gpibadd;		//


	char Reserve[16];	//
} Options;


class PULGEN {
private:
	void MakeBlank(double st, double l);
	void MakeTXGate(double st, double l);
	void MakeTX2( double st, double l );
	void MakeAUX1( double st, double l );
	void MakeQPSK( double st, double l, int phase );
	void MakeAUX2( double st, double l );
	void MakePowLevel( double st, double l, int pow );
	void MakeOSTrig( double st );
	void MakeMeter( double st );
	void MakeADTrig( double st );
	void MakeAUX3( double st, double l );
	void MakeComb( double st, double l );
	void Make1st( double st, double l );
	void Make2nd( double st, double l );
	void MakeAUX4( double st, double l );
	void MakeDATrig( double st );

//	void MakeLoop( double t, double a);
//	void MakeCmd( double t, uint32_t patt);
//	void Make1Bit( double st, double l, uint16_t mask, uint16_t pat);
	void MakeEXTTrig( double st );
	void MakePGON( double st, double l );
	void MakeEnd( double t );
	double MakeOneT1T2( double t);
	void init();	// ローカル変数の初期化
	int check();
public:
	PULDATC	mem;
	PULBRD 	brd;
	void Make1Bit( double st, double l, uint32_t mask, uint32_t pat);
	void MakeCmd( double t, uint32_t patt);
	void MakeCmd( double t, uint32_t patt, uint16_t patt2);
	void MakeLoop( double t, double a);
	void clear();
	void clear(double t);
	void make();
	void mem2brd();
	void dispmem(int fd, int stpt, int lines=20);
	void start(unsigned num=0);
	void stop();
	PULGEN( int add, void *dat );
	double pulselen();
	double duty();
	Options opt;
	int		isDouble;
	double	blank;
	int		cpn;
	double	cpw;
	double	cpi;
	double	fpw;
	double	spw;
	double	tj;
	double	t2;
	double	ADOFF;
	int		OSCILLO;		//TRG.OUT position 0=CS,1=CE,2=1S,3=1E, 4=same as A/D
	int		PowLevel;
	int		QPSKComb;
	int		QPSK1st;
	int		QPSK2nd;
	int		ADTrigLocation; // A/D trigger is 0:SPIN ECHO position 1:FID position
	int		EXTTRIG;
	int		BlankIsLoopTime;
	int		UseComb;
	int 	error;
	int		extmode;
	uint16_t	loopn;
	int		year, month, day, hour, minute, second;
	char	title[31];
	char	modified;	// memory transfer request
	char	res[49];
};

#define _PULGEN_H_
#endif

/*** EOF ***/
