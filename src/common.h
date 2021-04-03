//コモン
#ifndef _COMMON_H_
#define _COMMON_H_

#include "cgifunc.h"
#include "logging.h"

extern char SYSTEM_NAME[];

#define DATAPATH "/sd/fms/"
#define SD_BIN	"/sd/bin/"



void submenu(char *build_str);
void disparg(char *tt, int clen);


void passwd_main(void);
void mainmenu(int key);
void address_main();
void messages_main();
void check_log();
void network_setting();
void downloadlog_main();


typedef struct {
	char nickname[128];
	char address[128];

} ADDRESS_DATA;


typedef struct {
	long addr;	// Address.bin 0..63
	long cc;		// Address.bin 0..63
	long trig;	// 0: no use 1:up 2:down
	long spare[31];
	char title[128];
	char message[2048];
} MESSAGE_DATA;

typedef struct {

	unsigned long me_dhcp;
	unsigned long me_ip;
	unsigned long me_subnet;
	unsigned long me_gateway;

	unsigned long dns_dhcp;
	unsigned long dns1_ip;
	unsigned long dns2_ip;

	struct {
		char smtp_addr[128];
		long smtp_port;
		char pop3_addr[128];
		long pop3_port;
		char pop3_user[128];
		char pop3_pass[128];
		long smtp_auth;
	} mail[3];
	long ntp_use;
	char ntp_addr[128];
	long ntp_h, ntp_m, ntp_s;
	
	long baud;
	
	long spare[30];

} NET_SETTINGS;


extern NET_SETTINGS NS;
extern ADDRESS_DATA ADDRESS[64];
extern MESSAGE_DATA MESSAGE[32];
extern char *MSG_TITLE[32];


#endif

