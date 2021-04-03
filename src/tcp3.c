//
// telnet style COMMAND SERVER
// keep-alive server

/* gcc -lsocket -lnsl -o inet_server inet_server.c */


#ifdef _WIN32
#	include <winsock2.h>
#	include <ws2tcpip.h>
#else
#	include <sys/types.h>
#	include <sys/socket.h>
#	include <netinet/in.h>
#	include <arpa/inet.h>
#	include <netdb.h>
#endif

#include <errno.h>
#include <pthread.h>
#include <sched.h>

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "tcp.h"


#define CONNECT_DEBUG
//#define TCP_DEBUG




#define dprintf printf


#if 0
TCPÉTÅ[ÉoÅ[ÇÃäKëw
tcpserv3()
{
	accept_loop(){
		send_recv_thread(){
			execPULSER(){
				compu()
			}
		}
	}
}
#endif

static
int search_delimit( char *str )
{
	int ct;
	ct = 1;
	while( *str ){
		if( *str == (char)13 ) return ct;
		if( *str == (char)10 ) return ct;
		if( *str ==  ';'     ) return ct;
		str++;
		ct++;
	}
	return 0;
}

static
void stripCRLF_NULL( char *buf, int len )
{
	int i;
	for( i = 0 ; i < len ; i++){
		if( 13 == *buf || 10 == *buf || 0 == *buf) *buf=';';
		buf++;
	}
}


void dump( char *b, int n )
{
	int i;
	printf("\n----------------- %4d bytes\n", n);
	for( i = 0 ; i < n ; i++){
		printf("%02x ", b[i]);
	}
	printf("\n---------------------------\n");
}


///////////////////////////
// command interpreter
///////////////////////////
#define BUFF_SIZE (8200)
void* send_recv_thread(void *arg)
{
	int acc, ans;
	char *tcpbuf;
	char stm[200];
	char *cmdbuf;
	int cmdlen;
	int delimit;
	TCP3_ARGS *t3arg;
	void (*XXXexec)(char *,int, char **);

	t3arg = (TCP3_ARGS *)arg;
	XXXexec = t3arg->execrc;

	cmdbuf = (char *)malloc(1);
	if( NULL == cmdbuf ){ perror("out of memory"); exit(1); }
	tcpbuf = (char *)malloc(BUFF_SIZE+10);
	if( NULL == tcpbuf ){ perror("out of memory"); exit(1); }
	cmdlen = 0;
	pthread_detach(pthread_self());

	acc = t3arg->acc;
	/////////////////////////////////////////////////////////////////////////
	/* RECEIVE loop */
	for(;;) {
		delimit = 0;
		ans = recv( acc, tcpbuf, BUFF_SIZE, 0 );
		if( ans <= 0 ){
			goto discon;
		}
#ifdef TCP_DEBUG
		dump( tcpbuf, ans );
#endif
		tcpbuf[ans] = 0;
		stripCRLF_NULL( tcpbuf, ans );

		cmdbuf = (char *)realloc( cmdbuf, cmdlen + ans + 10 );	//  append RXdata
		if( NULL == cmdbuf ){ perror("out of memory"); exit(1); }
		memcpy( &cmdbuf[cmdlen], tcpbuf, ans );					//  append RXdata
		cmdlen += ans;											//  append RXdata
		cmdbuf[cmdlen] = 0;										//  append RXdata
//		printf("[%s]ans:%d cmdlen:%d\n", cmdbuf, ans, cmdlen );
		while( (1<=cmdlen)&& 0 != (delimit = search_delimit( cmdbuf ))){
//			printf("[%s]delimit=%d cmdlen:%d\n", cmdbuf, delimit, cmdlen );
			memcpy( stm, cmdbuf, delimit );
			stm[delimit] = 0;
			memcpy( cmdbuf, &cmdbuf[delimit], cmdlen-delimit+1 );
			cmdlen -= delimit;
//			printf("cmdlen=%d\n", cmdlen );
			if( delimit == 1 ) continue;
//			printf("recv:[%s]\n", stm );
			if( 1 < delimit){
				if( stm[0] == ';' ){
					// null command is skipped
				} else {
//					dprintf("execute[%s:%s]\n", t3arg->idn_msg, stm );
					(*XXXexec)(stm, acc, NULL);
				}
			} else {
				;
			}
		}
//		printf("next cmdlen:%d delimit:%d\n", cmdlen, delimit);
	}
	//////////////////////////////////////////////////////////////
discon:;
#ifdef CONNECT_DEBUG
	printf("-------------------disconnected\n");
#endif
	/* close socket */
	close(acc);
	free(cmdbuf);
	free(tcpbuf);
	free(t3arg);
	/* terminate thread */
	pthread_exit((void *)0);
	return 0;
}

////////////////////////////////////////////
// TCP connection waiting loop
////////////////////////////////////////////
void accept_loop(int soc, void (*exec_func)(char *,int, char **), const char *msg)
{
	char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
	struct sockaddr_storage from;
	int acc;
	socklen_t len;
	pthread_t thread_id;
	void *send_recv_thread(void *arg);

	TCP3_ARGS *t3arg;


	for (;;) {
		len = (socklen_t)sizeof(from);
		if ((acc = accept(soc, (struct sockaddr *)&from, &len)) == -1) {
			if(errno != EINTR){
				perror("accept_loop():accept");
			}
		} else {
			getnameinfo((struct sockaddr *)&from, len, hbuf, sizeof(hbuf), sbuf, sizeof(sbuf),
				NI_NUMERICHOST | NI_NUMERICSERV);

#ifdef CONNECT_DEBUG
			printf("accept_loop():connect from [%s:%s]\n", hbuf, sbuf);
#endif
			t3arg = (TCP3_ARGS *)calloc(sizeof(TCP3_ARGS),1 ); // free at thread
			if( NULL == t3arg ){ perror("out of memory"); exit(1); }
			t3arg->execrc = exec_func;
			t3arg->acc = acc;
			t3arg->idn_msg = msg;

			/* Create thread */
			if (pthread_create(&thread_id, NULL, send_recv_thread, (void *)t3arg ) != 0) {
				perror("err:accept_loop():pthread_create");
			} else {
//				printf("accept_loop():thread_id=%d\n", (int)thread_id);
			}
		}
	}
}



////////////////////////////////////////////
// Execute socket()Å®bind()Å®listen()
////////////////////////////////////////////
int server_socket(int port)
{
	int server_sockfd;
	int onflag;
    socklen_t server_len;
    struct sockaddr_in server_address;

    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);	//---------------- socket()
	onflag = 1;
	setsockopt( server_sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&onflag, sizeof(onflag));

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(port);

    server_len = sizeof(server_address);
	bind(server_sockfd, (struct sockaddr *)&server_address, server_len);//-----------------bind()

    listen(server_sockfd, 5);							//------------------listen()
	return server_sockfd;
}

int tcpserv3(int portnum, void (*execrc)(char *,int, char **), const char *id_msg )
{
	int soc;
	if ((soc = server_socket(portnum)) == -1) {
		printf("tcpserv3():server_socket(%d):error\n",portnum);
		return -1;
	}
	printf("Ready for accept TCP/IP port:%d\n", portnum);
	accept_loop(soc, execrc, id_msg);
	close(soc);
	return 0;	//OK
}
/*** EOF ***/

