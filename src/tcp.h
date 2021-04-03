//シンプルTCP通信
//
//
//
#ifndef _TCP_H_
#define _TCP_H_




typedef struct {
	int acc;
	int port;
	void (*execrc)(char *,int, char **);
	const char *idn_msg;
	void *func;
	int num1,num2;
} TCP3_ARGS;

int tcpserv3(int portnum, void (*execrc)(char *,int, char **), const char *id_msg);


void tcpserv(int port, void (*exec_func)(char *,int, char *) );
int sendtcp( const char *toHost, int toPort, const char *msg);
int querytcp( const char *toHost, int toPort, const char *msg, char *ans, int ansn );
int querytcp_delimit( const char *toHost, int toPort, const char *msg, char **alloced_ans, const char *delimit );
void suexec( char *cmd );



#endif

