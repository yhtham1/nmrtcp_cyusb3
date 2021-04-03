//
// N210-1026S,T GPIB interpreter
//
#include "compu.h"


static int debugMessages=0;
static void dprintf(const char* fmt, ...) // DEBUG printf DIPSW MASKED
{
	va_list  argptr;
	char str[1000];
	if( debugMessages ){
		va_start( argptr, fmt );
		vsnprintf( str, sizeof str, fmt, argptr );//FIX20201115
		printf(str);
		va_end( argptr );
	}
}


DV14	dv14;


void run_lua(const char *);

static
void send_addata(int fd, char **ans, uint32_t *wv, uint32_t nsample)
{
	char *o1buf, *o2buf;
	unsigned int i;

	o1buf = (char *)calloc(nsample+2,16);	//text buffer
	if( NULL == o1buf ){
		perror("out of memory");
		exit(1);
	}
	sprintf( &o1buf[0], "::SIZE:%08x:P", (nsample*16)+2 );//+2 is CRLF
	o2buf = &o1buf[16];
	for( i = 0 ; i < nsample ; i++){
		sprintf( &o2buf[i*16], "%08x%08x", wv[i*2+0], wv[i*2+1] );
	}
	ans_fd( fd, o1buf, ans );
	free( o1buf );
}




int compuAD(char *buf, int fd, char **ans)
{
	void ResetSaver( int );
	char *cmd, *cmd2, stm[200], *saveptr, *arg;
	int stts;
	stts = 0;
	strlwr( buf );
	saveptr = NULL;
	for( cmd2 = buf ; NULL != (cmd = MY_strtok_r( cmd2, ";", &saveptr)) ; cmd2 = NULL ){
		strcpy( stm, cmd );
		//////////////////////////////////////////////////
		if( NULL !=(arg = checkword( stm, "readstatus", &stts ))){
			char buf[10];
			int d;
			d = dv14.ReadStatus();
			sprintf(buf, "%d", d);
			ans_fd( fd, buf, ans );
			continue;
		}
		if( NULL !=(arg = checkword( stm, "*idn?", &stts ))){ 
			ans_fd( fd, IDN_MSG_AD, ans );
			continue;
		}
		if( NULL !=(arg = checkword( stm, "run_lua ", &stts ))){ 
			trimString(arg);
			dprintf("AD:run_lua:[%s]\n", arg );
			run_lua(arg);
			continue;
		}
		if( NULL !=(arg = checkword( stm, "startad ", &stts ))){
			uint32_t w, ite, cols; int32_t flip;
			w = 4096; ite = 1; cols = 1; flip=0;
			cutFourData( arg, &w, &ite, &cols, &flip );
			dprintf("StartAD(samples:%d,iter:%d,colums:%d,flip:%d)\n",
				w, ite, cols, flip
			);
			dv14.StartAD( w, ite, (uint16_t)cols, (uint16_t)flip );
			continue;
		}
		if( NULL !=(arg = checkword( stm, "readwave ", &stts ))){
			uint32_t *wv, nsample;
			uint32_t col, dmy;

			dprintf("--wavesize=%d\n", dv14.wavesize );
			nsample = dv14.wavesize;
			wv = (uint32_t *)calloc( nsample, 16 );
			cutThreeData( arg, &col, &dmy, &dmy );
			dprintf("--col=%d\n", col );
			stts = dv14.ReadWave( col, wv );
			if( -1 == stts ){ dprintf("not start\n"); }
			if(  0 == stts ){ dprintf("now sampling or trigger waiting\n"); }
			if( stts == 1){ //data ready
				send_addata( fd, ans, wv, nsample );
			};
			free( wv );
			continue;
		}
		if( NULL !=(arg = checkword( stm, "readmemoryb ", &stts ))){
			uint32_t *wv;
			uint32_t add, nsample, dmy;

			add = 0; nsample = 1;
			cutThreeData( arg, &add, &nsample, &dmy );

			wv = (uint32_t *)calloc( nsample, 2 * sizeof(uint32_t) );		//binary buffer
			dprintf("--add=%d\n", add );
			dprintf("--nsample=%d\n", nsample );

			stts = dv14.ReadMemory( add, nsample, wv );

			if( stts == 1){ //data ready
				send_addata( fd, ans, wv, nsample );
			}
			free( wv );
			continue;
		}
		if( NULL !=(arg = checkword( stm, "readmemory ", &stts ))){
			uint32_t *wv, nsample;
			uint32_t col, dmy;
			col = 1;
			nsample = dv14.wavesize;
			cutThreeData( arg, &col, &dmy, &dmy );
			dprintf("--wavesize=%d\n", nsample );
			dprintf("--col=%d\n", col );
			wv = (uint32_t *)calloc( nsample, 2*sizeof(uint32_t) );
			stts = dv14.ReadMemory( col, wv );

			if( stts == 1){ //data ready
				send_addata( fd, ans, wv, nsample );
			};
			free( wv );
			continue;
		}

		if( NULL !=(arg = checkword( stm, "getsamplefreq", &stts ))){
			uint32_t f;
			f = dv14.GetSampleFreq();
			char buf[10];
			sprintf(buf, "%d", f);
			ans_fd( fd, buf, ans );
			continue;
		}
		if( NULL !=(arg = checkword( stm, "gettriggercount", &stts ))){
			uint32_t f;
			f = dv14.GetTriggerCount();
			char buf[10];
			sprintf(buf, "%d", f);
			ans_fd( fd, buf, ans );
			continue;
		}
		if( NULL !=(arg = checkword( stm, "setsamplefreq ", &stts ))){
			uint32_t f, sf, dmy;
			char buf[10];
			cutThreeData( arg, &sf, &dmy, &dmy );
			f = dv14.SetSampleFreq(sf);
			sprintf(buf, "%d", f);
			ans_fd( fd, buf, ans );
		
			continue;
		}
		if( NULL !=(arg = checkword( stm, "dumptest ", &stts ))){
			uint32_t i; char *ob;
			uint32_t nsample, dmy;
			nsample = 1;
			if( 0 == cutThreeData( arg, &nsample, &dmy, &dmy )) continue;
			printf("dump test start\n");
			ob = (char *)calloc(nsample+1,16);
			for( i = 0 ; i < nsample*16 ; i++){
				ob[i] = 'A' + ((i/0x1000)&0x0f);
			}
			ans_fd( fd, ob, ans );
			free(ob);
			printf("dump test end\n");
			continue;
		}
		if( NULL !=(arg = checkword( stm, "outb ", &stts ))){
			int nn; uint32_t add, cd;
			uint32_t dt;
			nn = cutThreeData( arg, &add, &cd, &dt );
			if( nn == 2 ){
				dprintf("outb:0x%04x 0x%02x\n", add, cd );
				outdevb( (unsigned)add, (unsigned)cd );
			}
			continue;
		};
		if( NULL !=(arg = checkword( stm, "outw ", &stts ))){
			int nn; uint32_t add, cd;
			uint32_t dt;
			nn = cutThreeData( arg, &add, &cd, &dt );
			if( nn == 2 ){
				dprintf("outb:0x%04x 0x%04x\n", add, cd );
				outdevw( (unsigned)add, (unsigned)cd );
			}
			continue;
		};
		if( NULL !=(arg = checkword( stm, "inb ", &stts ))){
			int nn; uint32_t add, cd; char tbuf[10];
			uint32_t dt;
			nn = cutThreeData( arg, &add, &cd, &dt );
			if( nn == 1 ){
				int indevb( unsigned add );
				nn = indevb( (unsigned)add );
				dprintf("inb:0x%04x=0x%02x\n", add, nn );
				sprintf( tbuf, "%02X", nn );
				ans_fd( fd, tbuf, ans );
			}
			continue;
		};
		if( NULL !=(arg = checkword( stm, "inw ", &stts ))){
			int nn; uint32_t add, cd; char tbuf[10];
			uint32_t dt;
			nn = cutThreeData( arg, &add, &cd, &dt );
			if( nn == 1 ){
				int indevw( unsigned add );
				nn = indevw( (unsigned)add );
				dprintf("inw:0x%04x=0x%04x\n", add, nn );
				sprintf( tbuf, "%04X", nn );
				ans_fd( fd, tbuf, ans );
			}
			continue;
		};
		if( NULL !=(arg = checkword( stm, "ndebug", &stts ))){ debugMessages=0; continue; }
		if( NULL !=(arg = checkword( stm, "debug", &stts ))) { debugMessages=1; continue; }
	}
	return stts;
}
//
//call from send_recv_thread();
//
void execAD(char *msg, int fd, char **ans)
{
	compuAD( msg, fd, ans );
}


//thread for TCP/IP for AD
void* thread_ad_server(void* pParam) // TCP/IPˆ—(PORT:5026)
{
	printf("thread_ad_server():listen TCP/IP PORT:5026\n");
	for(;;){
		tcpserv3(5026, execAD, "AD");
	}
	return NULL;
}



int callAD(lua_State *L)
{
	const char *luacmd;
	char *cmd;
	char *ans;
	ans = (char *)malloc(2);
	if( NULL == ans ){ perror("out of memory"); exit(1); }
	luacmd = lua_tostring(L, 1);
	cmd = strdup( luacmd );
	strcpy(ans, "");
	execAD(  cmd, 0, &ans );
	free(cmd);
	if( 0 != strlen(ans)){
		lua_pushstring(L, ans );
		free(ans);
		return 1;
	}
	free(ans);
	return 0;
}




void initADBoard(int baseadd, char *idmsg)
{
	dv14.base_add = baseadd;
	dv14.setpara( idmsg );
}

/*** EOF ***/
