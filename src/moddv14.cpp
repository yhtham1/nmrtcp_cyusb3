//ＵＴＦ８
//
// DV14
//
#include "moddv14.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "inoutdev.h"


DV14::DV14()
{
	base_add  = 0x20;
	ramsize   = 524288;
	bits = 14;
	wavesize  = 256;
	flip      = 0;
	divide_n  = 0;
	prescaler = 0;
	ext_clock_freq = 25000000L;
	int_clock_freq = 25000000L;
	isEXTclock = 0;
	SetSampleFreq(int_clock_freq);//FIX20201111
}

uint32_t DV14::GetSampleFreq(int isEXTERNAL)
{
	uint32_t ans, f;
	f = (isEXTERNAL)? ext_clock_freq:int_clock_freq;
	ans = f/((divide_n)*(1<<prescaler));	//FIX20181226
	return ans;
}

uint32_t DV14::GetBoardFreq()
{
	uint32_t f;
	f = (isEXTclock)? ext_clock_freq:int_clock_freq;
	return f;
}

uint32_t DV14::GetSampleFreq()
{
	return GetSampleFreq(isEXTclock);
}


uint32_t DV14::SetSampleFreq(uint32_t freq)
{
	double iclk;
	double boardclk;
	int n, pres;

	pres = 0;
	n    = 0;
	iclk = GetBoardFreq();
	boardclk = iclk;
	if( iclk < freq ){
		freq = iclk;
		goto skip;
	}
	n = iclk/freq;
	while( 256 < n ){
		pres++;
		if( pres == 8 ){
			pres = 7;
			n = 255;
			goto skip;
		}
		iclk = boardclk/(1<<pres);
		n = iclk/freq;
	}
skip:;
	if( 0 == n ) n = 1;		//FIX20181226
//	divide_n = n-1;
	divide_n = n & 0xff;	//FIX20181226
	prescaler = pres;
	return GetSampleFreq();
}

///////////////////////////////////////////////////////////////////////
//////////////////////////////////interface HardWear depended /////////
void DV14::outb(unsigned offset, unsigned dat)
{
	outdevb( base_add+offset, dat );
}

void DV14::outw(unsigned offset, unsigned dat)
{
	outdevw( base_add+offset, dat );
}

int DV14::inb(unsigned offset)
{
	return indevb( base_add+offset );
}

int DV14::inw(unsigned offset)
{
	return indevw( base_add+offset );//FIX20150604
}

void DV14::inburst(unsigned offset, uint8_t *ans, int nbyte)
{
	InBurstB( base_add+offset, ans, nbyte );
}
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

void DV14::SetAddress( uint32_t add )
{
//	printf("DV14::SetAddress(%08x)\n", add );

	outw( 0x10, (add    ) & 0xffff );// cos RAM address
	outb( 0x12, (add>>16) & 0xffff );

	outw( 0x14, (add    ) & 0xffff );// sin RAM address
	outb( 0x16, (add>>16) & 0xffff );
}

uint16_t DV14::GetCOS()
{
	outb( 0x18, 0x00);
	return inw( 0x18 );
}

uint16_t DV14::GetSIN()
{
	outb( 0x1a, 0x00);
	return inw( 0x1a );
}




#define DW sizeof(uint32_t)

// 
// col:1..n
// 
#define USE_INBURST


int DV14::ReadMemory( uint32_t address, uint32_t nSamples, uint32_t *wv_org )
{
	unsigned int i, base;
	uint8_t *ubuf;
	uint32_t *wv;
	uint32_t w;


	outb( 1, 0 ); //sampling off
	SetAddress( address );

	ubuf = (uint8_t *)calloc( nSamples, 2 * DW );//(COS+SIN)xWidth
	if( NULL == ubuf ){
		perror("out of memory");
		exit(1);
	}
#ifdef USE_INBURST
	InBurstB( base_add+0x18, &ubuf[0],             nSamples * DW);	//COS
	InBurstB( base_add+0x19, &ubuf[nSamples * DW], nSamples * DW);	//SIN
#endif

	wv = &wv_org[0];

	for( i = 0 ; i < nSamples ; i++){
		uint32_t b0, b1, b2, b3;
#ifdef USE_INBURST
		b0 = ubuf[i*DW+0];
		b1 = ubuf[i*DW+1];
		b2 = ubuf[i*DW+2];
		b3 = ubuf[i*DW+3];
#else
		b0 = inb( 0x18 );
		b1 = inb( 0x18 );
		b2 = inb( 0x18 );
		b3 = inb( 0x18 );
#endif
		w = b3<<24 | b2<<16 | b1<<8 | b0;
		*wv = w; wv++; wv++;
	}
	wv = &wv_org[1];
	base = nSamples * DW ;
	for( i = 0 ; i < nSamples ; i++){
		uint32_t b0, b1, b2, b3;
#ifdef USE_INBURST
		b0 = ubuf[base+(i*DW)+0];
		b1 = ubuf[base+(i*DW)+1];
		b2 = ubuf[base+(i*DW)+2];
		b3 = ubuf[base+(i*DW)+3];
#else
		b0 = inb( 0x19 );
		b1 = inb( 0x19 );
		b2 = inb( 0x19 );
		b3 = inb( 0x19 );
#endif
		w = b3<<24 | b2<<16 | b1<<8 | b0;
		*wv = w; wv++; wv++;
	}
	free(ubuf);
	return 1;
}




int DV14::ReadMemory( int col, uint32_t *wv_org )
{
	int ans;
	ans = ReadMemory( (uint32_t)(wavesize*(col-1)), wavesize, wv_org );
	return ans;
}


// return value: status register
int DV14::ReadStatus()
{
	int d;
	d = inb( 0x00 );
	return d;
}



// 戻り値: 0:データ無し、1:データあり,-1:スタートしてないぞ
// return: 0:none data,1:exist data,-1:not triggerd
int DV14::ReadWave( int col, uint32_t *wv_org )
{
	int  d;
	if( 2 <= col ){
		goto skip1;
	}
	d = inb( 0x00 );
	if(  0 == isBUSY( d )){
		return -1;	//not start
	}
	if( !isADEND(d )){
		return 0;		//now sampling
	}
	col = 1;
skip1:;

	ReadMemory( col, wv_org );
	return 1;
}




// return value: 0:Not start、1:Now sampling ,2:data ready
int DV14::ReadWave( int col=1 )
{
	int d;

	if( 2 <= col ){
		goto skip1;
	}
	d = inb( 0x00 );
	if(  isBUSY( d )) return 0;	//not start
	if( !isADEND(d )) return 1;	//now sampling
	col = 1;
skip1:;
	outb( 1, 0 ); //sampling off
	SetAddress( (col-1) );
	return 2;
}



uint32_t DV14::GetTriggerCount()
{
	uint32_t ans;
	outb( 1,0);	//CONTROL stop sampling mode
	ans = inw( 0x0a );
	ans <<= 16;
	ans |= inw( 0x08 );
	return ans+1;
}



void DV14::StartAD( uint32_t samples, uint32_t it1, uint16_t col, uint16_t flip_on )
{
	int d;
	uint32_t r_samples;
//	outdevw(), indevb(), outdevb();
	cols = col;
	ites = it1;
	if( samples == 0 ) return;
	wavesize = samples;

	r_samples = samples;

	flip = flip_on;

	outb( 1,0);	//CONTROL stop sampling mode

	d = 0x20;//trigger is TTLUp edge
	d |= (0x07 & prescaler);
	if( flip       ) d |= 0x40;
	if( isEXTclock ) d |= 0x08;

	outb( 2 , d );			//MODE register
	outb( 3 , divide_n );	//clock divide

	outw( 4 , (r_samples    )&0xffff );		// block size bit15..0
	outb( 6 , (r_samples>>16)&0x00ff );		// block size bit23..16 FIX20150604

	outw( 8 , ((ites-1)     )&0xffff );		// iterate bit15..0
	outw( 10, ((ites-1)>>16 )&0xffff );		// iterate bit31..16

	outb( 0x0c, cols-1 );					// frame size
	SetAddress( 0 );

	outb( 1 , 1 ); // AD start

//	printf("StartAD(samples:%d,iter:%d,colums:%d,flip:%d 0x02:%02x 0x03:%02x )\n",
//		samples, ites, cols, flip , d, divide_n
//	);

}

int DV14::isBUSY(int a)
{
	return (a&2)?1:0;
}

int DV14::isADEND(int a)
{
	return (a&4)?1:0;
}
int DV14::getkeyword( const char *buf, const char *keyword, char *ans )
{
	int nk;
	char *ptst, *pted, *apt, *ptwd;
	nk = strlen(keyword);
	ptst = strstr( buf, keyword );
	if( NULL == ptst ) return -1;
	pted = strstr( &ptst[nk], "," );
	if( NULL == pted ){
		printf("internal error\n");
		return -1;
	}
	apt = ans;
	ptwd = &ptst[nk];
	while( ',' != *ptwd ){
		*apt = *ptwd;
		apt++;
		ptwd++;
	}
	*apt = '\0';


	return 0;
}

void DV14::setpara( const char *idstr )
{
	int stts;
	int clk;
	char buf[30];
	stts = getkeyword( idstr, "CLK=", buf );
	if( stts ){
		printf("DV14::setpara():error\n");
	} else {
		clk=(int)StrToHz(buf);
		int_clock_freq = (long)clk;
	}
	stts = getkeyword( idstr, "BIT=", buf );
	if( stts ){
		printf("DV14::setpara():error\n");
	} else {
		bits=atoi(buf);
//		printf("AD BITS=%d\n", bits );
	}
	stts = getkeyword( idstr, "RAM=", buf );
	if( stts ){
		printf("DV14::setpara():error\n");
	} else {
		ramsize=strtoul(buf, 0, 10);
//		printf("AD RAM=%d\n", ramsize );
	}
}





/*** EOF ***/
