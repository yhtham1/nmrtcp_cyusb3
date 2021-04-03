///////////////////////////////////////
// N210-1026S,T GPIB interpreter
///////////////////////////////////////

#include "compu.h"

#include <stdarg.h>
#include <stdio.h>


PULDATU pulmdmy[20];
//PULDATU *pulm;
PULGEN pul1(0x300,&pulmdmy[0]);



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


static
void send_pulserdata(int fd, char **ans, int start_add, uint32_t nsample)//FIX20201117
{
	char *o1buf, *o2buf;
	unsigned int i, add1;

	o1buf = (char *)calloc(nsample+2,16);	//text buffer
	if( NULL == o1buf ){
		perror("out of memory");
		exit(1);
	}
	sprintf( &o1buf[0], "::SIZE:%08x:P", (nsample*16)+2 );//+2 is CRLF
	o2buf = &o1buf[16];
	for( i = 0 ; i < nsample ; i++){
		add1 = start_add+i;
		sprintf( &o2buf[i*16], "%08x%08x",  pul1.mem.databuf[add1].t,pul1.mem.databuf[add1].d[0] );
	}
	ans_fd( fd, o1buf, ans );
	free( o1buf );
}

void run_lua(const char *);

// return value stts
//    0:command nothing
//    1:command processed
//    2:pulser running request flag
//    4:pulser stopping request flag
//    8:remake request flag
int compuPULSER(char *buf, PULGEN& p, int fd, char **ans)
{
	void ResetSaver( int );
	char *cmd, *cmd2, stm[200], *saveptr, *arg, abuf[30];
	int stts, s;
	stts = 0;
	strlwr( buf );
	saveptr = NULL;
	for( cmd2 = buf ; NULL != (cmd = MY_strtok_r( cmd2, ";", &saveptr)) ; cmd2 = NULL ){
		strcpy( stm, cmd );
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

		if( NULL !=(arg = checkword( stm, "makepulse ", &stts ))){
			int nn, ch; uint32_t pat; double st, ll ;
			nn = cutTimes( arg, &ch, &st, &ll );
			if( nn == 3 ){
				pat = 1u << ( ch & 0x1f );
				p.Make1Bit( st, ll, ~pat, pat );
				p.modified = 1;
			}
			continue;
		};
		if( NULL !=(arg = checkword( stm, "erasepulse", &stts ))){
			int nn, ch; uint32_t pat; double st, ll ;
			nn = cutTimes( arg, &ch, &st, &ll );
			if( nn == 3 ){
				pat = 1u << ( ch & 0x1f );
				p.Make1Bit( st, ll, ~pat, 0 );
				p.modified = 1;
			}
			continue;
		};
		if( NULL !=(arg = checkword( stm, "makecommand ", &stts ))){
			int nn; uint32_t cd;
			unsigned short dt; double st ;
			nn = cutParams( arg, &st, &cd, &dt );
			if( nn == 3 ){
				p.MakeCmd( st, cd, dt );
				p.modified = 1;
			}
			continue;
		};
		if( NULL !=(arg = checkword( stm, "poke ", &stts ))){
			int nn; uint32_t add, cd;
			uint32_t dt;
			nn = cutThreeData( arg, &add, &cd, &dt );
			if( nn == 3 ){
				p.mem.databuf[add].t = cd;
				p.mem.databuf[add].d[0] = dt&0xffff;
				p.mem.databuf[add].d[1] = dt>>16;
				p.modified = 1;
			}
			continue;
		};
		if( NULL !=(arg = checkword( stm, "peek ", &stts ))){
			int nn; uint32_t add, cd;char tbuf[40];
			uint32_t dt;
			nn = cutThreeData( arg, &add, &cd, &dt );
			if( nn == 1 ){
				cd = p.mem.databuf[add].t;
				dt = (p.mem.databuf[add].d[1]<<16) |( p.mem.databuf[add].d[0]&0xffff);
				sprintf( tbuf, "%08lX:%08lX,%08X", (unsigned long int)add, (unsigned long int)cd, (unsigned int)dt );
				ans_fd( fd, tbuf, ans );
			} else {
				sprintf( tbuf, "peek error %d", nn );
				ans_fd(fd, tbuf, ans );
			}
			continue;
		};

		if( NULL !=(arg = checkword( stm, "cpw ", 		&stts ))){ p.cpw = StrToTime( arg ); continue; }
		if( NULL !=(arg = checkword( stm, "cpi ", 		&stts ))){ p.cpi = StrToTime( arg ); continue; }
		if( NULL !=(arg = checkword( stm, "cpn ", 		&stts ))){ p.cpn = strtoul( arg, NULL, 0 ); continue;}
		if( NULL !=(arg = checkword( stm, "cpq ", 		&stts ))){ p.QPSKComb = strtoul( arg, NULL, 0 ); continue;}
		if( NULL !=(arg = checkword( stm, "usecomb ", 	&stts ))){ p.UseComb = (0!=strtoul( arg, NULL, 0 )); continue;}
		if( NULL !=(arg = checkword( stm, "fpw ", 		&stts ))){ p.fpw = StrToTime( arg ); continue; };
		if( NULL !=(arg = checkword( stm, "fpq ", 		&stts ))){ p.QPSK1st = strtoul( arg, NULL, 0 ); continue;}
		if( NULL !=(arg = checkword( stm, "tj ", 		&stts ))){ p.tj = StrToTime( arg ); continue; }
		if( NULL !=(arg = checkword( stm, "spw ", 		&stts ))){ p.spw = StrToTime( arg ); continue; };
		if( NULL !=(arg = checkword( stm, "spq ", 		&stts ))){ p.QPSK2nd = strtoul( arg, NULL, 0 ); continue;}
		if( NULL !=(arg = checkword( stm, "t2 ", 		&stts ))){ p.t2 = StrToTime( arg ); continue; }
		if( NULL !=(arg = checkword( stm, "adoff ", 	&stts ))){ p.ADOFF = StrToTime( arg ); continue; }
		if( NULL !=(arg = checkword( stm, "adtrg ",		&stts ))){ p.ADTrigLocation = strtoul( arg, NULL, 0 );continue; }
		if( NULL !=(arg = checkword( stm, "trgout ",	&stts ))){ p.OSCILLO = strtoul( arg, NULL, 0 );continue; }
		if( NULL !=(arg = checkword( stm, "exttrg ",	&stts ))){ p.EXTTRIG = strtoul( arg, NULL, 0 );continue; }
		if( NULL !=(arg = checkword( stm, "blank ",		&stts ))){ p.blank = StrToTime( arg ); continue; }
		if( NULL !=(arg = checkword( stm, "tx2predelay ", 	&stts ))){ p.opt.SENKOU_LEN = StrToTime( arg ); continue; };
		if( NULL !=(arg = checkword( stm, "tx2postdelay ", 	&stts ))){ p.opt.KOUKOU_LEN = StrToTime( arg ); continue; };
		if( NULL !=(arg = checkword( stm, "qpskdelay ", 	&stts ))){ p.opt.QPSKDELAY  = StrToTime( arg ); continue; };
		if( NULL !=(arg = checkword( stm, "freq ", 		&stts ))){ p.mem.pulserFreq = strtoul( arg, NULL, 0 ); continue;} //FIX20201204

		if( NULL !=(arg = checkword( stm, "cpw?", 		   &stts ))){ TimeToStrE( abuf, p.cpw             ); ans_fd( fd, abuf, ans ); continue; }
		if( NULL !=(arg = checkword( stm, "cpi?", 		   &stts ))){ TimeToStrE( abuf, p.cpi             ); ans_fd( fd, abuf, ans ); continue; }
		if( NULL !=(arg = checkword( stm, "cpn?", 		   &stts ))){  IntToStr(  abuf, p.cpn             ); ans_fd( fd, abuf, ans ); continue; }
		if( NULL !=(arg = checkword( stm, "cpq?", 		   &stts ))){  IntToStr(  abuf, p.QPSKComb        ); ans_fd( fd, abuf, ans ); continue; }
		if( NULL !=(arg = checkword( stm, "usecomb?",	   &stts ))){  IntToStr(  abuf, p.UseComb         ); ans_fd( fd, abuf, ans ); continue; }
		if( NULL !=(arg = checkword( stm, "fpw?", 		   &stts ))){ TimeToStrE( abuf, p.fpw             ); ans_fd( fd, abuf, ans ); continue; }
		if( NULL !=(arg = checkword( stm, "fpq?",		   &stts ))){  IntToStr(  abuf, p.QPSK1st         ); ans_fd( fd, abuf, ans ); continue; }
		if( NULL !=(arg = checkword( stm, "tj?", 		   &stts ))){ TimeToStrE( abuf, p.tj              ); ans_fd( fd, abuf, ans ); continue; }
		if( NULL !=(arg = checkword( stm, "spw?", 		   &stts ))){ TimeToStrE( abuf, p.spw             ); ans_fd( fd, abuf, ans ); continue; }
		if( NULL !=(arg = checkword( stm, "spq?",		   &stts ))){  IntToStr(  abuf, p.QPSK2nd         ); ans_fd( fd, abuf, ans ); continue; }
		if( NULL !=(arg = checkword( stm, "t2?", 		   &stts ))){ TimeToStrE( abuf, p.t2              ); ans_fd( fd, abuf, ans ); continue; }
		if( NULL !=(arg = checkword( stm, "adoff?", 	   &stts ))){ TimeToStrE( abuf, p.ADOFF           ); ans_fd( fd, abuf, ans ); continue; }
		if( NULL !=(arg = checkword( stm, "adtrg?",		   &stts ))){  IntToStr(  abuf, p.ADTrigLocation  ); ans_fd( fd, abuf, ans ); continue; }
		if( NULL !=(arg = checkword( stm, "trgout?",	   &stts ))){  IntToStr(  abuf, p.OSCILLO         ); ans_fd( fd, abuf, ans ); continue; }
		if( NULL !=(arg = checkword( stm, "exttrg?",	   &stts ))){  IntToStr(  abuf, p.EXTTRIG         ); ans_fd( fd, abuf, ans ); continue; }
		if( NULL !=(arg = checkword( stm, "blank?", 	   &stts ))){ TimeToStrE( abuf, p.blank           ); ans_fd( fd, abuf, ans ); continue; }
		if( NULL !=(arg = checkword( stm, "tx2predelay?",  &stts ))){ TimeToStrE( abuf, p.opt.SENKOU_LEN  ); ans_fd( fd, abuf, ans ); continue; }
		if( NULL !=(arg = checkword( stm, "tx2postdelay?", &stts ))){ TimeToStrE( abuf, p.opt.KOUKOU_LEN  ); ans_fd( fd, abuf, ans ); continue; }
		if( NULL !=(arg = checkword( stm, "qpskdelay?",    &stts ))){ TimeToStrE( abuf, p.opt.QPSKDELAY   ); ans_fd( fd, abuf, ans ); continue; }
		if( NULL !=(arg = checkword( stm, "single?",       &stts ))){  IntToStr(  abuf, (p.isDouble)?0:1        ); ans_fd( fd, abuf, ans ); continue; } //FIX20201201
		if( NULL !=(arg = checkword( stm, "double?",       &stts ))){  IntToStr(  abuf, (p.isDouble)?1:0        ); ans_fd( fd, abuf, ans ); continue; } //FIX20201201
		if( NULL !=(arg = checkword( stm, "waitmode?",     &stts ))){  IntToStr(  abuf, (p.BlankIsLoopTime)?0:1 ); ans_fd( fd, abuf, ans ); continue; } //FIX20201201
		if( NULL !=(arg = checkword( stm, "loopmode?",     &stts ))){  IntToStr(  abuf, (p.BlankIsLoopTime)?1:0 ); ans_fd( fd, abuf, ans ); continue; } //FIX20201201
		if( NULL !=(arg = checkword( stm, "setmode?",      &stts ))){  IntToStr(  abuf, (p.extmode)?1:0         ); ans_fd( fd, abuf, ans ); continue; } //FIX20201201
		if( NULL !=(arg = checkword( stm, "freq?",         &stts ))){  IntToStr(  abuf, p.mem.pulserFreq        ); ans_fd( fd, abuf, ans ); continue; } //FIX20201204

		if( NULL !=(arg = checkword( stm, "start ",		   &stts ))){
			unsigned long d;
			stts|= 2;	//start request
			stts|= 8;	//Remake request
			d = 0;
			if( 6 < strlen( stm )) d = strtoul( arg, NULL, 0 );
			if( 0xffff < d ) d = 0xffff;
			p.loopn = d;
			dprintf("PG:start command %d\n", p.loopn );
			continue;
		};

		if( NULL !=(arg = checkword( stm, "update",		   &stts ))){ //FIX20201201
			stts|= 8; // remake request
			continue;
		}
		if( NULL !=(arg = checkword( stm, "double",  &stts ))){ p.isDouble = 1; continue;};
		if( NULL !=(arg = checkword( stm, "single",  &stts ))){ p.isDouble = 0; continue;};
		if( NULL !=(arg = checkword( stm, "waitmode",&stts ))){ p.BlankIsLoopTime = 0;continue; }
		if( NULL !=(arg = checkword( stm, "loopmode",&stts ))){ p.BlankIsLoopTime = 1;continue; }
		if( NULL !=(arg = checkword( stm, "stop", &stts ))){ 
			dprintf("PG:stop\n");
			stts |=4; 
			continue;
		};
		//////////////////////////////////////////////////
		if( NULL !=(arg = checkword( stm, "*idn?", &stts ))){ 
			ans_fd( fd, IDN_MSG_PULSER, ans );
			continue;
		}
		if( NULL !=(arg = checkword( stm, "run_lua ", &stts ))){ 
			trimString(arg);
			dprintf("PG:run_lua:[%s]\n", arg );
			run_lua(arg);
			continue;
		}
		if( NULL !=(arg = checkword( stm, "memclr", &stts ))){ 
			p.clear();
			p.modified = 1;
			continue;
		};
		if( NULL !=(arg = checkword( stm, "blockwrite", &stts ))){
//			void blockwrite(PULGEN& p);
//			blockwrite(p);
//			p.modified = 1;
			continue;
		}
		if( NULL !=(arg = checkword( stm, "saver ", &stts ))){ s = strtoul( arg, NULL, 0 );ResetSaver( s ); continue; }
		if( NULL !=(arg = checkword( stm, "isrun?", &stts ))){
			if( p.brd.isRun() ) ans_fd( fd,  (char *)"RUN", ans );
			else ans_fd( fd, (char *)"STOP", ans );
			continue;
		}
		if( NULL !=(arg = checkword( stm, "setmode ", &stts ))){
			s = strtoul( arg, NULL, 0 );
			p.extmode = (s)?1:0;
			continue;
		}
		if( NULL !=(arg = checkword( stm, "dispmem", &stts ))){
			int nn; uint32_t st, num, dmy;
			nn = cutThreeData( arg, &st, &num, &dmy );
			switch(nn){
			case 1:pul1.dispmem(fd, st);		break;
			case 2:pul1.dispmem(fd, st, num);	break;
			default:;
				pul1.dispmem(fd, 0);
			}
			continue;
		}
		if( NULL !=(arg = checkword( stm, "readmemoryb ", &stts ))){//FIX20201117
			uint32_t add, numdat, dmy;
			add = 0; numdat = 1;
			cutThreeData( arg, &add, &numdat, &dmy );
			if(0 <= numdat && ((add+numdat) < MAX_PULMEM) ){
				send_pulserdata( fd, ans, add, numdat );
			}
			continue;
		}

		if( NULL !=(arg = checkword( stm, "hide", &stts ))){
//			s = strtoul( arg, NULL, 0 );
//			HideDisplay = ( s ) ? 1:0;
			continue;
		}
		if( NULL !=(arg = checkword( stm, "ndebug", &stts ))){ debugMessages=0; continue; }
		if( NULL !=(arg = checkword( stm, "debug", &stts ))) { debugMessages=1; continue; }
	}
	return stts;
}


void pulseUpdate()
{
//	int stts;
	static PULGEN old(0, pul1.mem.databuf);
//	stts = pul1.brd.isRun();

	if( pul1.extmode ){
		pul1.stop();
		pul1.mem2brd();
		return ;
	}


	if( 0 == memcmp( &pul1, &old, sizeof(pul1 ))){
		// not change
	} else {
//		if( !pul1.extmode){//FIX20150204
			pul1.mem.clearpulm();
			pul1.make();
			pul1.modified = 0;	//FIX20150204
			pul1.stop();
			pul1.mem2brd();
//			stts = 0;
//		}
		old = pul1;
	}
}


//
// -1:keep;(recalc)
// 0: Stop
// 1: Start
void pulseOnOff( int onoff )
{
//	int mod, stts;
	if( onoff ){
		pul1.start();
	} else {
		pul1.stop();
	}
}

//
//call from send_recv_thread();
//
void execPULSER(char *msg, int fd, char **ans)
{
	static uint32_t count=0;
	int stts;

	count++;
	stts = compuPULSER( msg, pul1, fd, ans );
	if( stts & 0x08 ){ pulseUpdate();  }
	if( stts & 0x02 ){ pulseOnOff( 1 );}
	if( stts & 0x04 ){ pulseOnOff( 0 );}
}

//thread for TCP/IP for PULSER
void* thread_pulser_server(void* pParam) // TCP/IPjobs(PORT:5025)
{
	printf("thread_pulser_server():listen TCP/IP PORT:5025\n");
	for(;;){
		tcpserv3( 5025, execPULSER, "PUL" );
	}
	return NULL;
}

int callPG(lua_State *L)
{
	const char *luacmd;
	char *cmd;
	char *ans;
	ans = (char *)malloc(2);
	if( NULL == ans ){ perror("out of memory"); exit(1); }
	luacmd = lua_tostring(L, 1);
	cmd = strdup( luacmd );
	strcpy(ans, "");
	execPULSER(  cmd, 0, &ans );
	free(cmd);
	if( 0 != strlen(ans)){
		lua_pushstring(L, ans );
		free(ans);
		return 1;
	}
	free(ans);
	return 0;
}





int getpulserpara(char *idmsg) // 0:OK -1:NG
{
	int stts;
	int clk;
	char buf[30];
	stts = getkeyword( idmsg, "CLK=", buf );

	if( stts ){
		printf("getpulserpara():error " __FILE__ " \n");
		pul1.mem.pulserFreq = 40000000ul;
		return -1; // NG
	} else {
//		printf("CLK=%s\n", buf );
		clk=(int)StrToHz(buf);
//		printf("CLK=%d\n", clk );
		pul1.mem.pulserFreq = (long)clk;
	}
	return 0; // OK
}

int initPulserBoard(int baseadd, char *idmsg)	// 0:OK -1:NG
{
	int stts;
	stts = -1;
	printf("MAX_PULMEM=%d ", MAX_PULMEM );
	fflush(stdout);
	PULDATU* pulm;
	pulm = (PULDATU *)calloc( sizeof(PULDATU),MAX_PULMEM );
	if( NULL == pulm ){ perror("out of memory"); exit(1); }
	pul1.mem.databuf = pulm;
	pul1.mem.u_max = MAX_PULMEM;
	pul1.mem.clearpulm();

	stts = getpulserpara(idmsg);//pul1.mem.extClock = 40000000ul;
	pul1.brd.base_address = baseadd;
	pul1.stop();
	pul1.clear();
	pul1.fpw = 0.1e-6;
	pul1.t2 = 100e-6;
	pul1.spw = 0.2e-6;
	pul1.isDouble = 0;

	pul1.blank = 1.0;

	pul1.make();
	pul1.extmode = 1;
	return stts;
}




/*** EOF ***/
