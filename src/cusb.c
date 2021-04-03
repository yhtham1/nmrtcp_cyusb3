//ＵＴＦ８
//
// CUSB.C
//          functions for CY7C68013
//
#include <windows.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include "ezusb.h"


volatile unsigned char INBURST_BUSY=0;
volatile unsigned char OUTBURST_BUSY=0;
static volatile unsigned char USB_BUSY=0;

static
int usbin()
{
	while( USB_BUSY ){
		Sleep(20);
	}
	USB_BUSY = 1;
//	if( isConf ){
//		printf("\n-------------------------\n");
//		printf(  "USB REQUEST IS CONFLICTED\n");
//		printf(  "-------------------------\n");
//	}
	return 0;
//	if( 0==USB_BUSY ){
//		return 0;
//	}
//	printf("-------------------------\n");
//	printf("USB REQUEST IS CONFLICTED\n");
//	printf("FATAL\n");
//	printf("-------------------------\n");
//	exit(1);
	return -1;
}

static
int usbout()
{
	USB_BUSY = 0;
	return 0;
}


void cusb_close(HANDLE h, HANDLE mh )
{
	if( 0 == h ) return;
	CloseHandle( h );
	if( mh ){
		CloseHandle(mh);
	}
}


BOOL cusb_get_string(HANDLE &h, int idx, char *s)
{
	BOOL result;
	DWORD numOfXfer;
	long i;
	char pvbuf[130];
	GET_STRING_DESCRIPTOR_IN sin;
	sin.Index = idx;
	sin.LanguageId = 27;
	numOfXfer = 0;
	usbin();
	result = DeviceIoControl(
		h,
		IOCTL_Ezusb_GET_STRING_DESCRIPTOR,
		&sin,
		sizeof(sin),
		pvbuf,
		sizeof(pvbuf),
		&numOfXfer,
		0
	);
//	printf("%ld %d\n", numOfXfer, pvbuf[0] );
	for( i = 0 ; i <= ((pvbuf[0]/2)-2); i++){
		s[i] = pvbuf[2+i*2];
	}
	s[i] = 0;
	usbout();
	return result;
}

BOOL cusb_halt(HANDLE h)
{
	BOOL ret;
	DWORD nbyte;
	VENDOR_REQUEST_IN cf;
	usbin();
	cf.bRequest = 0xA0;
	cf.wValue = 0xE600;
	cf.wIndex = 0x0;
	cf.wLength = 1;
	cf.bData = 1;
	cf.direction = 0;
	ret = DeviceIoControl(
		h, IOCTL_Ezusb_VENDOR_REQUEST, 
		&cf, sizeof(cf), 
		0, 0, &nbyte, 0
	);
	if(0 == ret ){ ret = -1;} else { ret = 0; }
	usbout();
	return ret;
}


BOOL cusb_run(HANDLE h)
{
	BOOL ret;
	DWORD nbyte;
	VENDOR_REQUEST_IN cf;
	usbin();
	cf.bRequest = 0xA0;
	cf.wValue = 0xE600;
	cf.wIndex = 0x0;
	cf.wLength = 1;
	cf.bData = 0;
	cf.direction = 0;
	ret = DeviceIoControl(
		h, IOCTL_Ezusb_VENDOR_REQUEST, 
		&cf, sizeof(cf), 
		0, 0, &nbyte, 0
	);
	if(0 == ret ){ ret = -1;} else { ret = 0; }
	usbout();
	return ret;
}



BOOL cusb_download(HANDLE h, char *buf, int n)
{
	BOOL ret;
	DWORD nbyte;
	usbin();
	ret = DeviceIoControl(
		h, IOCTL_Ezusb_ANCHOR_DOWNLOAD, 
		buf, n, 
		0, 0, &nbyte, 0
	);
	if(0 == ret ){ ret = -1;} else { ret = 0; }
	usbout();
	return ret;
}

BOOL cusb_resetpipe(HANDLE h, long p)
{
	BOOL ret;
	DWORD nbyte;
	usbin();
	ret = DeviceIoControl(
		h, IOCTL_Ezusb_RESETPIPE, 
		&p, sizeof(p), 
		0, 	0, 	&nbyte, 0
	);
	if(0 == ret ){ ret = -1;} else { ret = 0; }
	usbout();
	return ret;
}

BOOL cusb_bulk_write(HANDLE h, long pipe, uint8_t *buf, long n)
{
	BOOL ret;
	DWORD nbyte;
	BULK_TRANSFER_CONTROL cf;
	cf.pipeNum = pipe;
	usbin();
	ret = DeviceIoControl(
		h, IOCTL_Ezusb_BULK_WRITE, 
		&cf, sizeof(cf),
		buf, n, 
		&nbyte, 0
	);
	if(0 == ret ){ ret = -1;} else { ret = 0; }
	usbout();
	return ret;
}

BOOL cusb_bulk_read(HANDLE h, long pipe, uint8_t *buf, int n)
{
	BOOL ret;
	DWORD nbyte;
	BULK_TRANSFER_CONTROL cf;
	cf.pipeNum = pipe;
	usbin();
	ret = DeviceIoControl(
		h, IOCTL_Ezusb_BULK_READ, 
		&cf, sizeof(cf),
		buf, n, 
		&nbyte, 0
	);
	if(0 == ret ){ ret = -1;} else { ret = 0; }
	usbout();
	return ret;
}

int cusb_open( int n, HANDLE &h, HANDLE &mh)
{
	int ans;
	int i;
	HANDLE  tH;
	char name[100];
	char muName[100];
	usbin();
	mh = 0;
	if( 0 <= n ){
		sprintf(name, "\\\\.\\Ezusb-%d", n );
		h = CreateFile(
			name,
			GENERIC_READ|GENERIC_WRITE,
			FILE_SHARE_READ|FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			0L,
			0
		);
		if( h == INVALID_HANDLE_VALUE ){
			printf("cusb_open():open fail %s\n", name);
			ans = -1;
		} else {
			ans = 0;
		}
		usbout();
		return ans;
	}
	for( i = 0 ; i < 8 ; i++){
		sprintf(muName, "Ezusb-%d", i );
		tH = OpenMutex(MUTEX_ALL_ACCESS, 1, muName );
		if( tH ){
			CloseHandle(tH);
		} else {
			sprintf(name, "\\\\.\\Ezusb-%d", i );
			h = CreateFile(
				name,
				GENERIC_READ|GENERIC_WRITE,
				FILE_SHARE_READ|FILE_SHARE_WRITE,
				NULL,
				OPEN_EXISTING,
				0L,
				0
			);
			if( h == INVALID_HANDLE_VALUE ){
				//printf("INVALID\n");
			} else {
				mh = CreateMutex(0L, 0L, muName );
				goto skip;
			}
			mh = 0;
		}
	}
skip:;
	if( 7 <= i ){
		//printf("mutex NG\n");
		ans = (-1);
	} else {
		//printf("mutex OK\n");
		ans = 0;
	}
	usbout();
	return ans;
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//
// return code
//	0:none
//	1:OK
//	2:download ok please retry
//
int cusb_downloadFW(int n, HANDLE &h, HANDLE &mh, const char *str1, DWORD datenum)
{
	char fw[8192];
	DWORD num;
	char s1[100], s2[100];

	if( cusb_open( -1, h, mh )){
		//printf("open error\n");
		return 0;
	}
	cusb_get_string(h, 1, s1 );	//s1="F2FW"
	cusb_get_string(h, 2, s2 );
//	printf("[%s][%s]\n", s1, s2 );
	num = strtoul( s2, 0, 10 );
	if( 0 == strcmp( s1, str1 ) && datenum < num ){
//		printf("ready to go \n");
//		printf("%s:%ld\n", s1,num );
		// already loaded
		return 1;
	} else {
//		printf("download fx2fw.bix\n");
		FILE *fp;
		fp = fopen("fx2fw.bix", "rb");
		if( NULL == fp ){ perror("cusb_downloadFW():fx2fw.bix not found.\n"); exit(1);}
		fread( fw, 8192,1, fp );
		fclose(fp);
		printf("cusb_downloadFW():loading..\n");
		cusb_download( h, fw, 8192);
		Sleep(1000);
		printf("cusb_downloadFW():start CPU.\n");
		cusb_run(h);
		Sleep(1000);
		cusb_close( h, mh );
		printf("cusb_downloadFW():wait for CPU 2sec.\n");
		Sleep(2000);
	}
	return 2;
}

///////////////////////////////////////////// FX2FW functions below
void cusb_LED( HANDLE h, uint8_t onoff )
{
	uint8_t cmds[4];
	cmds[0] = USBCMD_LED;
	cmds[1] = (onoff)?(0x80):0x00;
	cusb_bulk_write( h, CPIPE, cmds, 2 );
}

//
// Answer USB bus connection speed
// return 0x80:HI-Speed(480M) 0x00:Full-Speed(12M))
int cusb_isSpeed( HANDLE h )
{
	uint8_t cmds[4];
	uint8_t buf[4];
	cmds[0] = USBCMD_USBCS;
	cusb_bulk_write( h, CPIPE, cmds, 1);
	cusb_bulk_read( h, RFIFO, buf, 1);
	return buf[0] & 0x80;
}

int cusb_isDIPSW( HANDLE h )
{
	uint8_t cmds[4];
	uint8_t buf[4];
	cmds[0] = USBCMD_DIPSW;
	cusb_bulk_write( h, CPIPE, cmds, 1);
	cusb_bulk_read( h, RFIFO, buf, 1);
	return buf[0];
}

//
// burst transfer
//
void cusb_bwrite(HANDLE h, uint8_t *buf , uint16_t num)
{
	uint8_t cmds[4];
	uint32_t i;
	i = 0;
	cmds[i] = USBCMD_BWRITE;   i++;
	cmds[i] = (num   ) & 0xff; i++;	  //LOW  tarnsfer size
	cmds[i] = (num>>8) & 0xff; i++;	  //HIGH tarnsfer size
	if( cusb_bulk_write(h, CPIPE, cmds, i)) { printf("cusb_bwrite(size:%d):error\n",num); cusb_close( h, 0); return; }
	if( cusb_bulk_write(h, TFIFO, buf, num)){ printf("cusb_bwrite(size:%d):error\n",num); cusb_close( h, 0); return; }
}


//
// Setup WAVEFORM TABLE for GPIF
//
// WAVE0 SingleRead
// WAVE1 SingleWrite
// WAVE2 BurstRead
// WAVE3 BurstWrite
//
// Method of create WAVEFORM TABLE file.
// 1.Generates a C-source by using the cypress "GPIF Designer".
// 2.Convert C-source to binary(WAVEFORM TABLES) by using the "gpifcnv.exe".
//
static //----------------------------------------------------------------------
void cusb_setwave(HANDLE h, const char *fn)
{
	uint8_t fw[1711];
	uint8_t buf[512];
	long off, j, i;
	int n;

	printf("----------- DOWNLOAD slow_dat.bin -------------\n");
	FILE *fp;
	fp = fopen( fn, "rb" );
	if( NULL == fp ){
		printf("cusb_setwave():file open error[%s]\n", fn);
		exit(1);
	}
	n = fread( fw, 1, sizeof(fw), fp );
	if( n == 0 ){
		printf("cusb_setwave():file read error\n");
		exit(1);
	}
	fclose(fp);

	//********************************************** IFCONFIG(8byte)
	i = 0;
	buf[i] = USBCMD_MODE; i++;
	buf[i] = USBMODE_GPIF + USBMODE_8BIT + USBMODE_ADDR + USBMODE_NOFLOW + USBMODE_DEBG;i++;
	buf[i] = USBCMD_GPIF; i++;
	off = 0;
	for( j = 0 ; j < 8 ; j++){
		buf[i] = fw[j + off];
		i++;
	}
	buf[i] = USBMODE_FLOW; i++;
	off = 8 + 32 * 4;
	for(j = 0 ; j < 36 ; j++){
		buf[i] = fw[j + off];
		i++;
	}
	if(cusb_bulk_write(h, CPIPE, buf, i)){
		exit(1);
	}
	//********************************************** WAVE0 SingleRead
	i = 0;
	buf[i] = USBCMD_WAVE0; i++;
	off = 8 + 32 * 0;
		for( j = 0 ; j < 32 ; j++){
		buf[i] = fw[j + off];
		i++;
	}
	if(cusb_bulk_write(h, CPIPE, buf, i)){
	}
	//********************************************** WAVE1 Single Write
	i = 0;
	buf[i] = USBCMD_WAVE1; i++;
	off = 8 + 32 * 1;
		for( j = 0 ; j < 32 ; j++){
		buf[i] = fw[j + off];
		i++;
	}
	if(cusb_bulk_write(h, CPIPE, buf, i)){
	}
	//********************************************** WAVE2 BurstWrite
	i = 0;
	buf[i] = USBCMD_WAVE2; i++;
	off = 8 + 32 * 2;
	for( j = 0 ; j < 32 ; j++){
		buf[i] = fw[j + off];
		i++;
	}
	if(cusb_bulk_write(h, CPIPE, buf, i)){
	}
	//********************************************** WAVE3 BurstWrite
	i = 0;
	buf[i] = USBCMD_WAVE3; i++;
	off = 8 + 32 * 3;
	for( j = 0 ; j < 32 ; j++){
		buf[i] = fw[j + off];
		i++;
	}
	if(cusb_bulk_write(h, CPIPE, buf, i)){
	}
	//**********************************************
}


//
// Set I/O address
//
static //----------------------------------------------------------------------
void cusb_swrite(HANDLE h, uint8_t dat)
{
	uint8_t cmds[10];
	int i;
    i = 0;
    cmds[i] = USBCMD_SWRITE; 	i++;
    cmds[i] = dat; 				i++;
	cusb_bulk_write(h, CPIPE, cmds, i);
}

//
//
//
static //----------------------------------------------------------------------
void cusb_sread(HANDLE h, uint8_t *dat)
{
	uint8_t cmds[10];
	uint8_t buf[10];
	int i;
    i = 0;
    cmds[i] = USBCMD_SREAD;		i++;
	cusb_bulk_write(h, CPIPE, cmds, i);
	cusb_bulk_read(h, RFIFO,  buf, 1);
    *dat = buf[0];
}



static //----------------------------------------------------------------------
int cusb_inb(HANDLE h, int add )
{
	uint8_t buf[10];
    cusb_swrite(h, (uint8_t)add);
    cusb_sread( h, buf);
    return buf[0];
}

static //----------------------------------------------------------------------
int cusb_inw(HANDLE h, int add )
{
	int ans;
	uint8_t H, L;
    cusb_swrite(h, (uint8_t)(add  )); // I/O address low byte
    cusb_sread( h, &L);				// read low byte
    cusb_swrite(h, (uint8_t)(add+1)); // I/O address high byte
    cusb_sread( h, &H);				// read high byte
	ans =(H<<8)|L;
    return ans;
}

static //----------------------------------------------------------------------
void cusb_outb(HANDLE h, int add, int dat )
{
	uint8_t cmds[10];
	int i;
	i = 0;
	cmds[i] = add;				i++;
	cmds[i] = dat;				i++;
	cusb_bwrite( h, cmds, i );
}

static //----------------------------------------------------------------------
void cusb_outw(HANDLE h, int add, int dat )
{
	uint8_t cmds[10];
	int i;
	i = 0;
	cmds[i] = add;				i++;
	cmds[i] = dat;				i++;
	cmds[i] = add+1;			i++;
	cmds[i] = (dat>>8);			i++;
	cusb_bwrite( h, cmds, i );
}

static //----------------------------------------------------------------------
void cusb_outburst(HANDLE h, uint8_t *buf, uint16_t bytes )
{
	cusb_bwrite( h, buf, bytes );
}

static //----------------------------------------------------------------------
void cusb_inburst(HANDLE h, int portadd, uint8_t *ans, int nbyte)
{
	const int BLOCK_SIZE = 4096;
	int i;
	int ans_p;
	uint8_t i_buf[BLOCK_SIZE+4];
	uint8_t cmds[10];

	cusb_swrite( h, portadd );	// set IN PORT ADDRESS
	i = 0;
	cmds[i] = USBCMD_BREAD; 			i++;
	cmds[i] = (BLOCK_SIZE   ) & 0xff;	i++;
	cmds[i] = (BLOCK_SIZE>>8) & 0xff;	i++;
	ans_p = 0;
	do {
		if( nbyte < BLOCK_SIZE ){
			cusb_bulk_write(h, CPIPE, cmds, 3);
			cusb_bulk_read(h, RFIFO, &i_buf[0], BLOCK_SIZE  );
			memcpy( &ans[ans_p], i_buf, nbyte );
			return;
		} else {
			cusb_bulk_write(h, CPIPE, cmds, 3);
			cusb_bulk_read(h, RFIFO, &ans[ans_p], BLOCK_SIZE);
		}
		nbyte -= BLOCK_SIZE;
		ans_p += BLOCK_SIZE;
	} while(0 < nbyte);
	//ちょうどnByteが０
}




static //----------------------------------------------------------------------
HANDLE cusb_search()
{
	int  a1;
	HANDLE h, mh;

	if( sizeof(uint32_t) != 4 ){
		printf("bit size error sizeof(uint32_t)=%d\n", sizeof(uint32_t));
		exit(-1);
	}
//	printf("sizeof int %d\n", sizeof(int));

	a1 = cusb_downloadFW(-1, h, mh, "F2FW", 20070600L);
	if( 0 == a1 ) return 0;	// not found
	if( a1 == 2 ){
		a1 = cusb_downloadFW(-1, h, mh, "F2FW", 20070600L);
	}
	if( a1 == 0 ) return 0;
	return h;
}


///////////////////////////////////////////////////////////// USBIO uio
///////////////////////////////////////////////////////////// USBIO uio
///////////////////////////////////////////////////////////// USBIO uio


typedef struct {
	HANDLE h;
} USBIO;

static USBIO uio_db[0x10];
static int WRCT, WRON;
static HANDLE BURST_H;
static uint8_t *OUTBUF;

//
// address to HANDLE
//
static
HANDLE uio_add2h(int add)
{
	add &= 0x0f00;
	return uio_db[add>>8].h;
}

int uio_inb(int add)
{
	HANDLE h;
	h = uio_add2h(add);
	if( 0 == h ) return 0x100;//FIX20181213
	return (int)cusb_inb( h, add & 0xff );
}

int uio_inw(int add)
{
	HANDLE h;
	h = uio_add2h(add);
	if( 0 == h ) return 0;
	return (int)cusb_inw( h, add & 0xff );
}

void uio_outb(int add, int dat)
{
	HANDLE h;
	h = uio_add2h(add);
	if( 0 == h ) return;

	if( WRON ){
		if(BURST_H == 0){
			BURST_H = h;
		}
		if( BURST_H != h ){
			printf("BURST_H error\n");
			exit(1);
		}
		OUTBUF[WRCT]=add; WRCT++;
		OUTBUF[WRCT]=dat; WRCT++;
	} else {
		cusb_outb( h, add & 0xff, dat & 0xff );
	}
}


void uio_outw(int add, int dat)
{
	HANDLE h;
	h = uio_add2h(add);
	if( 0 == h ) return;

	if( WRON ){
		if(BURST_H == 0){
			BURST_H = h;
		}
		if( BURST_H != h ){
			printf("BURST_H error\n");
			exit(1);
		}
		OUTBUF[WRCT]= add     & 0xff; WRCT++;
		OUTBUF[WRCT]= dat     & 0xff; WRCT++;
		OUTBUF[WRCT]=(add+1)  & 0xff; WRCT++;
		OUTBUF[WRCT]=(dat>>8) & 0xff; WRCT++;
	} else {
		cusb_outw( h, add & 0xff, dat & 0xffff );
	}
}

void uio_outBurstStart()
{
	while( OUTBURST_BUSY ){
		Sleep(10);
	}
	OUTBURST_BUSY = 1;
	WRCT = 0;
	WRON = 1;
	BURST_H = 0;
}

void uio_outBurstFlush()
{
	if( WRCT ){
		cusb_outburst(BURST_H, OUTBUF, WRCT );
	}
//	printf("uio_outBurstFlush():WRCT:%d\n", WRCT );
	WRCT = 0;
	WRON = 0;
	BURST_H = 0;
	OUTBURST_BUSY = 0;
}


//
// ans need 256bytes
//
void uio_GetIDN(int add, char *ans)
{
	int i, c;
	*ans = 0;
	if( 0 == uio_add2h( add )) return;
	for( i = 0 ; i < 256 ; i++){
		c = uio_inb( add );
		if( 0 == c ) break;
	}
	if( 250 < i ) return;
	for( i = 0 ; i < 250 ; i++){
		*ans = uio_inb( add );
		if( 0 == *ans) return ;
		ans++;
	}
}






void uio_inburst(int portadd, uint8_t *ans, int nbyte)
{
	HANDLE h;
	h = uio_add2h(portadd);
	if( 0 == h ) return;
	while(INBURST_BUSY){
		Sleep(10);
	}
	INBURST_BUSY = 1;
	cusb_inburst( h, portadd, ans, nbyte );
	INBURST_BUSY = 0;
}

//
// search all IF099 board
//
void uio_init(const char *wave_filename)
{
	int i, d;
	HANDLE hh;
	memset( uio_db, 0, sizeof(uio_db));
	for( i = 0 ; i < 16 ; i++){//uio 0x000..0xfff
		d = 0;
		hh = cusb_search();
		if( hh ){
			d = cusb_isDIPSW( hh );
//			cusb_LED(hh, 1 );
			d &= 0x0f;
			uio_db[d].h = hh;
			cusb_setwave( hh, wave_filename);
		}
	}
	printf("---------- USB INTERFACE INFORMATION ----------\n");
	printf("address search for board:IF099B\n");
	for( i = 0 ; i < 16 ; i++){
//		printf( "%1x00-%1xff:h=%-8d ", i, i, (int)uio_db[i].h );
		printf( "%1x00-%1xff:%-8.8s ", i, i, 
		(uio_db[i].h)?"EXIST":"-----" );
		if( (i & 0x03) == 0x03 ) printf("\n");
		if( uio_db[i].h ){
			cusb_LED( uio_db[i].h, 1 );
		}
	}
	printf("\n");
	OUTBUF = (uint8_t *)calloc(65540,1);
	if( NULL == OUTBUF ){
		perror("out of memory cusb.c:uio_init():OUTBUF\n");
		exit(1);
	}
}

void testled( int brdid, int onoff)
{
		if( uio_db[brdid].h ){
			cusb_LED( uio_db[brdid].h, onoff );
		}
}

void uio_close(void)
{
	int i;
	free(OUTBUF);
	WRCT = 0;
	for( i = 0 ; i < 16 ; i++){
		if( uio_db[i].h ){
			cusb_close(uio_db[i].h, 0 );
		}
	}
}


#if 0
int main(int argc, char **argv)
{
	int i;
	char idn[1024];
	uio_init("slow_dat.bin");
	uio_GetIDN( 0x1f, idn );
	printf("[%s]\n", idn );

	for(i = 0 ; i < 100 ; i++){
		uio_outb( 13, 0 );
		uio_outb( 12, 2 );
		uio_outw(  8, 0xffff );	//bit 0..15
		uio_outw( 14, 0xffff );	//bit16..31
		uio_outb( 12, 0 );
		Sleep(100);
		uio_outb( 13, 0 );
		uio_outb( 12, 2 );
		uio_outw(  8, 0x0000 );	//bit 0..15
		uio_outw( 14, 0x0000 );	//bit16..31
		uio_outb( 12, 0 );
		Sleep(100);
		printf("%d\n", i );
	}

	return 0;
}
#endif

/*** EOF ***/
