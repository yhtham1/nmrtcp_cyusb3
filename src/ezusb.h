//ＵＴＦ８
#ifndef _EZUSB_H_
#define _EZUSB_H_

#include "types.h"

const uint32_t IOCTL_Ezusb_GET_STRING_DESCRIPTOR = 0x222044;
const uint32_t IOCTL_Ezusb_ANCHOR_DOWNLOAD = 0x22201C;
const uint32_t IOCTL_Ezusb_VENDOR_REQUEST = 0x222014;
const uint32_t IOCTL_Ezusb_BULK_WRITE = 0x222051;
const uint32_t IOCTL_Ezusb_BULK_READ = 0x22204E;
const uint32_t IOCTL_Ezusb_RESETPIPE = 0x222035;



typedef struct {
   UCHAR    Index;
   USHORT   LanguageId;
} GET_STRING_DESCRIPTOR_IN, *PGET_STRING_DESCRIPTOR_IN;

typedef struct _VENDOR_REQUEST_IN
{
    BYTE    bRequest;
    WORD    wValue;
    WORD    wIndex;
    WORD    wLength;
    BYTE    direction;
    BYTE    bData;
} VENDOR_REQUEST_IN, *PVENDOR_REQUEST_IN;

typedef struct {
	DWORD pipeNum;
} BULK_TRANSFER_CONTROL;


/*** FX2FW Ver1.01 by OPTIMIZE ***/
const uint32_t TFIFO = 0;             //OUT2    TXFIFO
const uint32_t CPIPE = 1;             //OUT8    COMMAND
const uint32_t RFIFO = 2;             //IN6     RXFIFO

const uint8_t USBCMD_MODE = 0x80;	//mode(bit2-0) 000:PIO 001:FIFO 010:GPIF
								//data_bus(bit3) 0:8bit 1:16bit, use_addr(bit4) 0:No 1:Yes
								//flow_ctrl(bit5) 0:No 1:Yes, state_debug(bit6) 0:No 1:Yes

const uint8_t USBMODE_PIO    = 0x0;
const uint8_t USBMODE_FIFO   = 0x1;
const uint8_t USBMODE_GPIF   = 0x2;
const uint8_t USBMODE_8BIT   = 0x0;
const uint8_t USBMODE_16BIT  = 0x8;
const uint8_t USBMODE_ADDR   = 0x10;
const uint8_t USBMODE_NOADDR = 0x0;
const uint8_t USBMODE_FLOW   = 0x20;
const uint8_t USBMODE_NOFLOW = 0x0;
const uint8_t USBMODE_DEBG   = 0x40;
const uint8_t USBMODE_NODEBG = 0x0;

const uint8_t USBCMD_GPIF = 0x81;      //IFCONFIG(1)+RDYCTL(7)=8byte
const uint8_t USBCMD_WAVE = 0x82;      //WAVE(32*4)=128byte
const uint8_t USBCMD_FLOW = 0x83;      //FLOW(9*4)=36byte
const uint8_t USBCMD_WAVE0 = 0x84;     //WAVE 32byte
const uint8_t USBCMD_WAVE1 = 0x85;     //WAVE 32byte
const uint8_t USBCMD_WAVE2 = 0x86;     //WAVE 32byte
const uint8_t USBCMD_WAVE3 = 0x87;     //WAVE 32byte
const uint8_t USBCMD_WFSEL = 0x88;     //bit1-0:FIFORD, bit3-2:FIFOWR, bit5-4:SINGLERD, bit7-6:SINGLEWR
const uint8_t USBCMD_ADSET = 0x89;     //byte0:GPIFADRL, byte1:GPIFADRH
const uint8_t USBCMD_CPUCS = 0x8A;     //SPEED(bit4-3) 00:12MHz 01:24MHz 10:48MHz , CLKOE(bit1) 0:no 1:out
const uint8_t USBCMD_USBCS = 0x8B;     //return 1byte(bit7 1:HI-Speed(480M) 0:Full-Speed(12M))
const uint8_t USBCMD_SREAD = 0x90;     //return (8bit mode:1byte data, 16bit mode:2byte data(H -> L))
const uint8_t USBCMD_SWRITE = 0x91;    //8bit mode:1byte data, 16bit mode:2byte data(H -> L)
const uint8_t USBCMD_BREAD = 0x92;     //byte0:words(Low), byte1:words(Hi)
const uint8_t USBCMD_BWRITE = 0x93;    //byte0:words(Low), byte1:Words(Hi)
const uint8_t USBCMD_OEA = 0x0;        //bitx 0:input 1:output
const uint8_t USBCMD_OEB = 0x1;        //bitx 0:input 1:output
const uint8_t USBCMD_OEC = 0x2;        //bitx 0:input 1:output
const uint8_t USBCMD_OED = 0x3;        //bitx 0:input 1:output
const uint8_t USBCMD_OEE = 0x4;        //bitx 0:input 1:output
const uint8_t USBCMD_INA = 0x5;        //return port
const uint8_t USBCMD_INB = 0x6;        //return port
const uint8_t USBCMD_INC = 0x7;        //return port
const uint8_t USBCMD_IND = 0x8;        //return port
const uint8_t USBCMD_INE = 0x9;        //return port
const uint8_t USBCMD_OUTA = 0xA;       //output port
const uint8_t USBCMD_OUTB = 0xB;       //output port
const uint8_t USBCMD_OUTC = 0xC;       //output port
const uint8_t USBCMD_OUTD = 0xD;       //output port
const uint8_t USBCMD_OUTE = 0xE;       //output port
const uint8_t USBCMD_WAIT = 0xF;       //wait (byte0)ms
const uint8_t USBCMD_NOP = 0x10;       //wait (byte0
const uint8_t USBCMD_DIPSW = 0x11;
const uint8_t USBCMD_LED = 0x12;

#endif

