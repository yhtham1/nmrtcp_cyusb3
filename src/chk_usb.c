


#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "types.h"
#include "inoutdev.h"

int brdadd;



void init()
{
	int stts;
	FILE *fp;
	fp = fopen( "CHK.INI", "rt" );
	if( fp == NULL ){
		printf( "file does not exist CHK.INI\n");
		exit(1);
	}
	stts = fscanf( fp, "%x", &brdadd );
	fclose( fp );
	if( stts == 0 ){
		printf("file read error CHK.INI\n");
		exit(1);
	}
}



void outboard(int add, uint32_t pat)
{
	printf(" out bit = %04X\n", pat );
	outdevb( 13, 0 );
	outdevb( 12, 2 );
	outdevw( 8, pat&0xffff );
	outdevw( 14, pat>>16 );
	outdevb( 12, 0 );
}


void play(int c, int add )
{
	unsigned short pat;
	if( 32 == c ){
		outboard( add, 0 );
		return ;
	}
	if( '0' <= c && c <= '9' ){
		pat = 1 << (c-'0');
		outboard( add, pat );
		return;
	}
	if( 'a' <= c && c <= 'f' ){
		pat = 1 << (c-'a'+10);
		outboard( add, pat );
		return;
	}
}


int main()
{
	int c;
	brdadd = 0x300;

	open_io_dev();

	init();
	printf("check PULSER bus is USB\n");
	printf(" base address = %04x\n", brdadd );
	printf("\t0..9=bit 0..9\n\ta..f=bit 10..15\n\t(Esc)=Quit\n");
	for(;;){
		c = getchar();
		if( 27 == c ) exit(0);
		play( c, brdadd );
	}
	close_io_dev();
	return 0;
}


/*** EOF ***/
