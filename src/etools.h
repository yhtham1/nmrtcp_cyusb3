//ＵＴＦ８
// ＥＴＯＯＬＳ
void IntToStr( char *buf, int d );
double StrToTime(char *str);
void TimeToStr( char *buf, double t );
void TimeToStrE( char *buf, double t );
void HzToStr( char *buf, double t );
double StrToHz(char *str);
#ifndef _WIN32
#include <string.h>
char* strlwr(char *str);
char* strupr( char *str);
void Sleep(int);
#endif
