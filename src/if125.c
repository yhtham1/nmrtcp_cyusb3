//
///////////////////////////////////////////////////////////
// IF125基板用    IF125基板を用いてPC104:USBバスを読み書き
///////////////////////////////////////////////////////////


// 修正履歴
//
//
//

int inb104(int add);						// arm460用関数
void outb104(int add, unsigned char dat);	// arm460用関数


#define PC104_ADD		(0x0300)		//IF125のDIPSW

#define ADRS_REG	(PC104_ADD+2)
#define DATA_REG	(PC104_ADD)

void outdevb( unsigned add, unsigned dat )
{
	outb104( ADRS_REG, add );
	outb104( DATA_REG, dat &  0xff );
}

void outdevw( unsigned add, unsigned dat )
{
	outb104( ADRS_REG, add );
	outb104( DATA_REG, dat &  0xff );
	outb104( ADRS_REG, add+1 );
	outb104( DATA_REG, dat >> 8 );
}

int indevb( unsigned add )
{
	outb104( ADRS_REG, add );
	return inb104( DATA_REG );
}




/*** EOF ***/
