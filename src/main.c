//ＵＴＦ－８
//////////////////////////////////////////
// TCP PULSER for MinGW
//////////////////////////////////////////


// 修正履歴
//
//
//
//20201111 起動後 ad-getsamplefreqを行うとこけるのを修正
//20201115 軽微なBUGFIX vsnprintf使用, 64bit版の作成
//20201117 pulserにreadmemorybコマンドを追加
//20201201 コマンド追加 single?, double?等
//20201203 fx2fw.bix, slow_dat.binファイルエラーの追加
//20201204 freqコマンドの追加(ＨＷ無しでのダミーデバイス動作用)
//20210401 detect pulser error, adboard error
#define REVISION_TXT "VERSION 20210401"




#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#else

#include <sys/socket.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <termio.h>

#endif

#include <sys/stat.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

#include "inoutdev.h"
#include <signal.h>
#include <pthread.h>
#include <sched.h>
#include "tcp.h"
#include "pulser.h"
#include "pulgen.h"
#include "compu.h"
#include <lua.hpp>

char *IDN_MSG_PULSER;
char *IDN_MSG_AD;
char *IDN_MSG_RF;

void pul1Redraw()
{
}

char SYSTEM_NAME[1024];
char *MSG_TITLE[32];


int touroku(void * (*start_routine)(void *))
{
	pthread_attr_t tattr;
	pthread_t tid;

	struct sched_param param;

	int stts;
	int newprio = -20;
	
	/* デフォルト属性で初期化する */
	stts = pthread_attr_init (&tattr);
	
	/* 既存のスケジューリングパラメータを取得する */
	stts = pthread_attr_getschedparam (&tattr, &param);

//	printf("pri=%d\n",
//		param.sched_priority
//	);
	/* 優先順位を設定する。それ以外は変更なし */
	param.sched_priority = newprio;

	/* 新しいスケジューリングパラメータを設定する */
	stts = pthread_attr_setschedparam (&tattr, &param);
	if( stts != 0 ) printf("pthread_attr_setschedparam %d\n", stts);
	/* 指定した新しい優先順位を使用する */
	stts = pthread_create (&tid, &tattr, start_routine, NULL); 
	if( stts != 0 ) printf("pthread_create %d\n", stts);
    return 0;
}

int GetIDN( int add, char *buf )
{
	int i, c;
	char *p;
	buf[0] = 0x00;
	for( i = 0 ; i < 256 ; i++){
		c = indevb( add );
		if( c == 0x100 ) goto end1;
		if( c ==  0 ) goto ok1;
	}
	goto end1;

ok1:;
	p = buf;
	for( i = 0 ; i < 256 ; i++){
		c = indevb( add );
		*p = c;
		if( c == 0 ) goto ok2;
		p++;
	}
	goto end1;

ok2:;
	return 0;
end1:;
	return -1;
}


int SearchIDN(int add_off, const char *keyword, char *ans )
{
	int add, i;
	char buf[280];

	for( i = 0 ; i < 16 ; i++){
		add = i*256 + add_off;
		GetIDN( add, buf );
//		printf("SearchIDN:%d[%s]\n", add, buf );
		if( NULL != strstr( buf, keyword )){
//			printf("SearchIDN:found:0x%04x[%s]\n", add, buf );
			if( NULL != ans ){
				strcpy( ans, buf );
			}
			return add;
		}
	}
	return -1;
}


void outdevb(int add, unsigned dat)
{
	outdevb( (unsigned)add, dat );
}


#define log_printf printf
//
// シグナル捕獲関数
//
int CLOSE_REQUEST;

void recv_fail(int a)
{
	log_printf(" response timeout error recv_fail()\n");
}

//void recv_kill(int a)
//{
//	log_printf(" RX SIGKILL()\n");
//	CLOSE_REQUEST = 1;
//}

void recv_term(int a)
{
	log_printf(" RX SIGTERM()\n");
	CLOSE_REQUEST = 1;
}

void recv_int(int a)
{
	log_printf(" RX SIGINT()\n");
	CLOSE_REQUEST = 1;
}

int lua_outb(lua_State *L)
{
	int a, b;
	a = lua_tointeger(L,1);
	b = lua_tointeger(L,2);
	outdevb( a, b );
	return 0;
}
int lua_outw(lua_State *L)
{
	int a, b;
	a = lua_tointeger(L,1);
	b = lua_tointeger(L,2);
	outdevw( a, b );
	return 0;
}

int lua_inb(lua_State *L)
{
	int a, ans;
	a = lua_tointeger(L,1);
	ans = indevb(a);
	lua_pushinteger(L, ans);
	return 1;
}

int lua_inw(lua_State *L)
{
	int a, ans;
	a = lua_tointeger(L,1);
	ans = indevw(a);
	lua_pushinteger(L, ans);
	return 1;
}

int lua_OutBurstStart(lua_State *L)
{
	OutBurstStart();
	return 0;
}

int lua_OutBurstFlush(lua_State *L)
{
	OutBurstFlush();
	return 0;
}

int wait1ms(lua_State *L)
{
	int n;
	n = lua_tointeger(L, 1);
	Sleep(n);
	return 0;
}


void* thread_pulser_server(void* pParam);	// TCP/IP処理(PORT:5025)
void* thread_ad_server(void* pParam); 		// TCP/IP処理(PORT:5026)
void* thread_rf_server(void* pParam);		// TCP/IP処理(PORT:5027)

int callAD(lua_State *L);
int callRF(lua_State *L);
int callPG(lua_State *L);

#if 1
static const luaL_Reg taglist[] = {
		{"wait1ms",			&wait1ms			},
		{"inb",  			&lua_inb			},
		{"inw",  			&lua_inw			},
		{"outb", 			&lua_outb			},
		{"outw", 			&lua_outw			},
		{"OutBurstStart",	&lua_OutBurstStart	},
		{"OutBurstFlush",	&lua_OutBurstFlush	},
		{"callPG",			&callPG				},
		{"callAD",			&callAD				},
		{"callRF",			&callRF				},
		{ NULL, NULL }
	};


int my_system_register(lua_State *L)
{
	lua_pushglobaltable(L);
	luaL_setfuncs(L, taglist,0 );
	lua_pop(L,1);
	return 1;
}
#else
void my_system_register(lua_State *L)
{
	lua_register(L, "wait1ms",			wait1ms );
	lua_register(L, "inb",  			lua_inb  );
	lua_register(L, "inw",  			lua_inw  );
	lua_register(L, "outb", 			lua_outb );
	lua_register(L, "outw",				lua_outw );
	lua_register(L, "OutBurstStart",	lua_OutBurstStart );
	lua_register(L, "OutBurstFlush", 	lua_OutBurstFlush );
	lua_register(L, "callPG", 			callPG   );
	lua_register(L, "callAD", 			callAD   );
	lua_register(L, "callRF", 			callRF   );
}
#endif


//
//
//
int main(int argc, char **argv)
{
	int i, j;
//	putenv( "TZ=jst-9" );

//	signal( SIGKILL, recv_kill); NOT SUPPORT SIGKILL
//	signal( SIGTERM, recv_term);
//	signal( SIGINT, recv_int);

#ifdef _WIN32
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2 ,0), &wsaData) == SOCKET_ERROR) {
		printf ("Error initialising WSA.\n");
		return -1;
	}
#endif

	void fx2lp_init();//for CyUSB3.sys
	fx2lp_init();

	printf("\n******************************************************************\n");
//	printf(" TCP to USB CONVERTER for PULSER/AD/RFCONTROL " __TIME__ " " __DATE__ );
	printf(" TCP to USB CONVERTER for PULSER/AD/RFCONTROL "  REVISION_TXT    );
	printf("\n******************************************************************\n\n");

	open_io_dev();

	printf("---------- CONNECTED BOARD INFORMATION ----------\n");
	for( i = 0x00 ; i < 0x100 ; i+=0x20 ){
		int OK;
		char buf[256];
		int stts, add ;
		OK = 0;
		for( j = 0 ; j < 0x10 ; j++){//FIX20181213
			add = (0x100*j)+i+0x1f;
			stts = GetIDN(add, buf);
			if( 0 == stts ){
				printf("usbGetIDN(0x%03X):[%s]\n", add, buf);
				OK = 1;
			}
		}
		if( 0 == OK ){
			printf("usbGetIDN(0x%03X):-----------------------------------------------\n", add);
		}
	}
	printf("\n");
	char *p;

	p = (char *)malloc( 1024 );
	if( NULL == p ){ perror("out of memory"); exit(1); }
	strcpy( p, "THAMWAY,N210-1026T PULSER,Version ");
	strcat( p,  VERSION );
	strcat( p,  "," );
	IDN_MSG_PULSER = p;

	p = (char *)malloc( 1024 );
	if( NULL == p ){ perror("out of memory"); exit(1); }
	strcpy( p, "THAMWAY,N210-1026T AD,Version ");
	strcat( p,  VERSION );
	strcat( p,  "," );
	IDN_MSG_AD = p;

	p = (char *)malloc( 1024 );
	if( NULL == p ){ perror("out of memory"); exit(1); }
	strcpy( p, "THAMWAY,N210-1026T RF,Version ");
	strcat( p,  VERSION );
	strcat( p,  "," );
	IDN_MSG_RF = p;

	{
		int initPulserBoard(int baseadd, char *idmsg);
		int add;
		char buf[280];
		printf("initPulserBoard START .. " );
		fflush(stdout);
		strcpy(buf, "" );
		add = SearchIDN( 0x1f, "PG32U", buf );//FIX20181213
		if( 0 < add ){
			strcat( IDN_MSG_PULSER, buf );
		}
//		printf("PUL:%d[%s]\n", add, buf );
		initPulserBoard(add & 0xff00 , IDN_MSG_PULSER );
		if( 0 <= add ){//FIX20210401
			printf("initPulserBoard OK\n" );//FIX20210401
		} else {
			printf("initPulserBoard ERROR\n" );
		}
	}

	{
		void initADBoard(int baseadd, char *idmsg);
		int add;
		char buf[280];
		printf("initADBoard START .." );
		fflush(stdout);
		strcpy(buf, "" );
		add = SearchIDN( 0x3f, "DV14U", buf );
		strcat( IDN_MSG_AD, buf );
//		printf("A/D:%d[%s]\n", add, buf );
		initADBoard( (add & 0xff00)+0x20, IDN_MSG_AD);
		if( 0 <= add){//FIX20210401
			printf("initADBoard OK\n" );//FIX20210401
		} else {
			printf("initADBoard ERROR\n" );
		}
	}

	{
		extern int RF_IOADD;
		int add;
		char buf[280];
		printf("RF controller: start\n" );
		int SearchRFMOD(int add_off, const char *keyword, char *ans );
		add = SearchRFMOD( 0xa0, "THAMWAY", buf );
		RF_IOADD = add;
		if( add != 0 ){
			printf("RF controller address:0x%04x [%s]\n", add, buf );
		} else {
			printf("RF controller:not found\n");
		}
		printf("RF controller: OK\n" );
	}
	printf("\n");

	touroku( thread_pulser_server );
	Sleep(100);
	touroku( thread_ad_server );
	Sleep(100);
	touroku( thread_rf_server );

	Sleep(300);
	printf("\n");
	{	//FIX20201115
		int *a;
		if( sizeof(a) == 8 ){
			printf("NMRTCP Windows 64bit version build:"  __TIME__ " " __DATE__ "\n");
		} else {
			printf("NMRTCP Windows 32bit version build:"  __TIME__ " " __DATE__ "\n");
		}
	}

#ifdef _WIN32
	int main_lua(int argc, char **argv);
	main_lua(argc, argv );
#else
	for(;;){
		Sleep(1000);
	}
#endif

#ifdef _WIN32
	WSACleanup();
#endif
	close_io_dev();
	return 0;

}

/*** EOF ***/
