//
// N210-1026S,T GPIB interpreter
//
#include "compu.h"

#define _POSIX_SOURCE  1   /* POSIX comliant source (POSIX)*/


int RF_IOADD = 0x00;	//0x6a0(protII) or  0x0a0(protIII)


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

void run_lua(const char *);



static void sendmodu( const char *msg, int  add)
{
	char cmd[100], *p;
	strcpy( cmd, msg );
//	printf("sendmodu():[%s]\n", msg );
	strcat(cmd,"\r\n");
	p = &cmd[0];
	while( *p ){
		outdevb( add, *p );
		p++;
	}
}

static void recvmodu( char *ans, int add)
{
	char *p;
	int c, ct;
	ct=0;
	p = ans;
	*p = 0;

	for(;;){
		c = indevb( add );
//		Sleep(1);
		if( c == 0x100 ) return ;//FIX20181213
		if( c == 0 || c == 255 ) return ;
		*p = c;
		p++;
		ct++;
		if( 250 < ct ) return;// over flow
		*p = 0;
	}
}

int compuRF(char *buf, int fd, char **strans)
{
	void ResetSaver( int );
	char *cmd, *cmd2, stm[200], *saveptr, *arg;
	char tbuf[258];
	int stts;
	stts = 0;
	strlwr( buf );
	saveptr = NULL;
	for( cmd2 = buf ; NULL != (cmd = MY_strtok_r( cmd2, ";", &saveptr)) ; cmd2 = NULL ){
		strcpy( stm, cmd );
		//////////////////////////////////////////////////
//		if( NULL !=(arg = checkword( stm, "*idn?", &stts ))){ 
//			ans_fd( fd, IDN_MSG_RF, strans );
//			continue;
//		}
		if( NULL !=(arg = checkword( stm, "run_lua ", &stts ))){ 
//			ans_fd(fd, "RUN_LUA!!", ans);
			trimString(arg);
			dprintf("RF:run_lua:[%s]\n", arg );
			run_lua(arg);
			continue;
		}
		if( NULL !=(arg = checkword( stm, "ndebug", &stts ))){ debugMessages=0; continue; }
		if( NULL !=(arg = checkword( stm, "debug", &stts ))) { debugMessages=1; continue; }
		strupr( stm );
		if( RF_IOADD ){
			sendmodu( stm, RF_IOADD );
		} else {
			return -1;//error
		}
		Sleep(1);
		recvmodu( tbuf, RF_IOADD );
		ans_fd( fd, tbuf,strans );
	}
	return stts;
}


//
//call from send_recv_thread();
//
void execRF(char *msg, int fd, char **ans)
{
	compuRF( msg, fd, ans );
}


//thread for TCP/IP for RF controller
void* thread_rf_server(void* pParam) // TCP/IPˆ—(PORT:5027)
{
	printf("thread_rf_server():listen TCP/IP PORT:5027\n");
	for(;;){
		tcpserv3(5027, execRF, "RF");
	}
	return NULL;
}



//
// return value RF MSG address 
//
int SearchRFMOD(int add_off, const char *keyword, char *ans )
{
	int add, i;
	char rxbuf[280];
	for( i = 0 ; i < 16 ; i++){
		add = i*256 + add_off;
		printf("search RFMOD %d \r", i );
		fflush(stdout);
		sendmodu( "*IDN?", add );
		Sleep(50);
		recvmodu( rxbuf,   add );
		if( NULL != strstr( rxbuf, keyword )){
			if( NULL != ans ){
				strcpy( ans, rxbuf );
			}
//			printf("found add:%d\n", add );
			return add;
		}
	}
	*ans = 0;
	return 0;
}

int callRF(lua_State *L)
{
	const char *luacmd;
	char *cmd;
	char *ans;
	ans = (char *)malloc(2);
	if( NULL == ans ){ perror("out of memory"); exit(1); }
	luacmd = lua_tostring(L, 1);
	cmd = strdup( luacmd );
	strcpy(ans, "");
	execRF(  cmd, 0, &ans );
	free(cmd);
	if( 0 != strlen(ans)){
		lua_pushstring(L, ans );
		free(ans);
		return 1;
	}
	free(ans);
	return 0;
}



/*** EOF ***/
