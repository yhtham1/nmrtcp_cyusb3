//ＵＴＦ－８
//////////////////////////////////////////
// TCP PULSER for MinGW
//////////////////////////////////////////


// 修正履歴
//
//
//

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

#include <setupapi.h>
#include <CyAPI.h>
#include <cyioctl.h>

#include <locale.h>



static void dump_xmit(char *p)
{
	int i, j;
	for( j = 0 ; j < 4 ; j++){
		printf("%04X:", 0xffff & (j*16) );
		for( i = 0 ; i < 8 ; i++){
			printf("%02X ", 0xff & p[i+j*16] );
		}
		printf("-- " );
		for( i = 8 ; i < 16 ; i++){
			printf("%02X ", 0xff & p[i+j*16] );
		}
		printf("\n");
	}
}

int check_FriendlyName( HANDLE hDevice, TCHAR *targetName )
{
	int ans;
	ans = 0;
	DWORD dwBytes = 0;
//	ULONG len = 256;
//	UCHAR *buf = new UCHAR[len];
//	DeviceIoControl(hDevice, IOCTL_ADAPT_GET_DEVICE_NAME,	buf, len,	buf, len,	&dwBytes, NULL);
//	_tprintf("IOCTL_ADAPT_GET_DEVICE_NAME:     [%s]\n", buf );
//	delete[] buf;

	PUCHAR FriendlyName = new UCHAR[256];
	DeviceIoControl(hDevice, IOCTL_ADAPT_GET_FRIENDLY_NAME,	FriendlyName, 256,	FriendlyName, 256,	&dwBytes, NULL);
	printf("IOCTL_ADAPT_GET_FRIENDLY_NAME:   [%s]\n", FriendlyName );

	if( 0 == memcmp(FriendlyName, "Cypress FX2LP No EEPROM Device", 30 )){
		printf("found FX2FW\n");
		ans = 1;
	}
	delete[] FriendlyName;

//	UCHAR endPts;
//	DeviceIoControl(hDevice, IOCTL_ADAPT_GET_NUMBER_ENDPOINTS,		NULL, 0,		&endPts, sizeof (endPts),		&dwBytes, NULL);
//	_tprintf("IOCTL_ADAPT_GET_NUMBER_ENDPOINTS:[%d]\n", endPts );

	return ans;
}

void fx2lp_download(HANDLE hDevice)
{
	int bufLen = 8192;
	int TimeOut = 3;

	int iXmitBufSize = sizeof(SINGLE_TRANSFER) + bufLen; // The size of the two-part structure
	UCHAR *pXmitBuf = new UCHAR[iXmitBufSize]; // Allocate the memory
	ZeroMemory(pXmitBuf, iXmitBufSize);

	UCHAR *pROM = pXmitBuf + sizeof(SINGLE_TRANSFER);
	FILE *fp;
	fp = fopen( "fx2fw.bix", "rb" );
	if( NULL == fp ){
		perror("ERROR:   file:fx2fw.bix is not found.");//FIX20201203
		exit(1);
	}
	fread( pROM, 8192, 1, fp );
	fclose(fp);

	PSINGLE_TRANSFER pTransfer = (PSINGLE_TRANSFER)pXmitBuf; // The SINGLE_TRANSFER comes first
	pTransfer->SetupPacket.bmRequest = 0x40;			//bmReq;
	pTransfer->SetupPacket.bRequest  = 0xa0; 			//ReqCode;
	pTransfer->SetupPacket.wValue    = 0; 				//Value;
	pTransfer->SetupPacket.wIndex    = 0; 				//Index;
	pTransfer->SetupPacket.wLength   = bufLen;			//送信データ長
	pTransfer->SetupPacket.ulTimeOut = TimeOut / 1000;
//	pTransfer->Reserved              = 0;
	pTransfer->ucEndpointAddress     = 0x00;			// Control pipe
	pTransfer->IsoPacketLength       = 0;
	pTransfer->BufferOffset          = sizeof (SINGLE_TRANSFER);
	pTransfer->BufferLength          = bufLen;
	
	DWORD dwReturnBytes;
	BOOL ret;

//	dump_xmit( (char *)pXmitBuf );

	ret = DeviceIoControl (hDevice, IOCTL_ADAPT_SEND_EP0_CONTROL_TRANSFER,
		pXmitBuf, iXmitBufSize,
		pXmitBuf, iXmitBufSize,
		&dwReturnBytes, NULL);
	if( ret == 0 ){
//		printf("fx2lp_download(%d):fail\n", (int)hDevice);
		printf("fx2lp_download:fail\n");
	} else {
//		printf("fx2lp_download(%d):success\n", (int)hDevice);
		printf("fx2lp_download:success\n");
	}
}

void fx2lp_run(HANDLE hDevice)
{
	int bufLen = 1;
	int TimeOut = 3;

	int iXmitBufSize = sizeof(SINGLE_TRANSFER) + bufLen; // The size of the two-part structure
	UCHAR *pXmitBuf = new UCHAR[iXmitBufSize]; // Allocate the memory
	ZeroMemory(pXmitBuf, iXmitBufSize);

	PSINGLE_TRANSFER pTransfer = (PSINGLE_TRANSFER)pXmitBuf; // The SINGLE_TRANSFER comes first
	pTransfer->SetupPacket.bmRequest = 0x40;			//bmReq;
	pTransfer->SetupPacket.bRequest  = 0xa0; 			//ReqCode;
	pTransfer->SetupPacket.wValue    = 0xe600; 			//Value;
	pTransfer->SetupPacket.wIndex    = 0; 				//Index;
	pTransfer->SetupPacket.wLength   = 1;				//送信データ長
	pTransfer->SetupPacket.ulTimeOut = TimeOut / 1000;
//	pTransfer->Reserved              = 0;
	pTransfer->ucEndpointAddress     = 0x00;			// Control pipe
	pTransfer->IsoPacketLength       = 0;
	pTransfer->BufferOffset          = sizeof (SINGLE_TRANSFER);
	pTransfer->BufferLength          = bufLen;
	
	DWORD dwReturnBytes;
	BOOL ret;
	ret = DeviceIoControl (hDevice, IOCTL_ADAPT_SEND_EP0_CONTROL_TRANSFER,
		pXmitBuf, iXmitBufSize,
		pXmitBuf, iXmitBufSize,
		&dwReturnBytes, NULL);
	if( ret == 0 ){
//		printf("fx2lp_run(%d):fail\n", (int)hDevice);
		printf("fx2lp_run:fail\n");
	} else {
		printf("fx2lp_run:success\n");
	}
}


void fx2lp_init()
{
	HANDLE hDevice;
	SP_DEVINFO_DATA devInfoData;
	SP_DEVICE_INTERFACE_DATA devInterfaceData;
	PSP_INTERFACE_DEVICE_DETAIL_DATA functionClassDeviceData;
	ULONG requiredLength = 0;
	int deviceNumber = 0; // Can be other values if more than 1 device connected to driver
	HDEVINFO hwDeviceInfo = SetupDiGetClassDevs( (LPGUID)&CYUSBDRV_GUID,	NULL,	NULL,	DIGCF_PRESENT|DIGCF_INTERFACEDEVICE);
	if (hwDeviceInfo != INVALID_HANDLE_VALUE) {
		devInterfaceData.cbSize = sizeof(devInterfaceData);
		while( SetupDiEnumDeviceInterfaces( hwDeviceInfo, 0, (LPGUID) &CYUSBDRV_GUID,	deviceNumber, &devInterfaceData)) {
			printf("CyUSB3.sys---------------------------------------\n");
			SetupDiGetInterfaceDeviceDetail( hwDeviceInfo, &devInterfaceData, NULL, 0,	 &requiredLength, NULL);
			ULONG predictedLength = requiredLength;
			functionClassDeviceData 		= (PSP_INTERFACE_DEVICE_DETAIL_DATA)malloc(predictedLength);
			functionClassDeviceData->cbSize = sizeof (SP_INTERFACE_DEVICE_DETAIL_DATA);
			devInfoData.cbSize 				= sizeof(devInfoData);
			if (SetupDiGetInterfaceDeviceDetail (hwDeviceInfo,	&devInterfaceData,	functionClassDeviceData,	predictedLength,	&requiredLength,	&devInfoData)) {
//				_tprintf("DevicePath:%s\n", functionClassDeviceData->DevicePath );
				hDevice = CreateFile (functionClassDeviceData->DevicePath,	GENERIC_WRITE | GENERIC_READ,	FILE_SHARE_WRITE | FILE_SHARE_READ,	NULL,	OPEN_EXISTING,	FILE_FLAG_OVERLAPPED,	NULL);
				free(functionClassDeviceData);
				if( check_FriendlyName( hDevice, NULL )){
					printf("----------- DOWNLOAD fx2fw.bix --------------\n");
					fx2lp_download( hDevice );
					printf("----------- EXECUTE  fx2fw.bix --------------\n");
					fx2lp_run( hDevice );
					Sleep(3000);//ＰＣでの認識の時間
				}
			}
			deviceNumber++;
		}
	}
	SetupDiDestroyDeviceInfoList(hwDeviceInfo);
}

/*** EOF ***/
