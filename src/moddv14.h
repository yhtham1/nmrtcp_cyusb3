//ＵＴＦ８
//
// DV14
//
#include <stdint.h>
#include "inoutdev.h"
#include "etools.h"

class DV14
{
public:
	DV14();
	int 		ReadStatus();
	uint16_t 	GetCOS();
	uint16_t 	GetSIN();
	int 		ReadWave(int col );
	int 		ReadWave(int col, uint32_t *wv );
	int 		ReadMemory(int col, uint32_t *wv );
	int			 ReadMemory( uint32_t address, uint32_t nSamples, uint32_t *wv_org );
	uint32_t 	GetTriggerCount();
	void 		StartAD( uint32_t samples, uint32_t ite, uint16_t cols, uint16_t flip );
	int 		isBUSY(int a);
	int 		isADEND(int a);
	void 		SetAddress( uint32_t add );
	int 		wavesize;
	int 		cols;
	uint32_t 	ites;
	int 		flip;
	uint32_t	GetSampleFreq();
	uint32_t	GetBoardFreq();
	uint32_t	GetSampleFreq(int isEXTERNAL);
	uint32_t	SetSampleFreq( uint32_t freq );
	uint32_t	GetIteration();
	void 		setpara(const char *idmsg );
	int 		base_add;//0xnn20

private:
	uint32_t ramsize;
	int bits;
	int getkeyword( const char *buf, const char *keyword, char *ans );
	int isEXTclock;
	uint32_t	ext_clock_freq;
	uint32_t	int_clock_freq;
	void outb(unsigned add, unsigned dat);
	void outw(unsigned add, unsigned dat);
	int inb( unsigned off );
	int inw( unsigned off );
	void inburst(unsigned offset, uint8_t *ans, int nbyte);
	int prescaler;
	int divide_n;

};





/*** EOF ***/
