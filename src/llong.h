#ifndef _LLONG_H_

#if 0

//
// llong.h 64bit 整数計算モジュール
//

typedef unsigned short ushort;
typedef unsigned long ulong;

#if 1
void llongAddllong( long* yh, long* yl, long xh, long xl );
void llongSubllong( long* yh, long* yl, long xh, long xl );
void llongMulllong( long* yh, long* yl); //yhyl = yh * yl : 64 = 32 x 32
#else
extern "C" {
	void llongAddllong( long* yh, long* yl, long xh, long xl );
	void llongSubllong( long* yh, long* yl, long xh, long xl );
	void llongMulllong( long* yh, long* yl); //yhyl = yh * yl : 64 = 32 x 32
	};
#endif

class llong {
public:
	long dath,datl;
	void add( long h, long l );
	void sub( long h, long l );
	llong();
	llong(ulong);
	friend llong operator- (const ulong& , const llong& );
	llong  operator- ( const llong& other );
	llong  operator* ( const llong& other );
	llong& operator+=( const llong& other );
	llong  operator+ ( const ulong& other );
	llong  operator+ ( const int& other );
	llong& operator= ( llong& other );
	llong& operator= ( const ulong& other );
	llong& operator= ( const double& other );
	llong& operator= ( const long& other );
	llong& operator= ( const int& other );
	int operator<=( const llong& other );
	int operator==( const llong& other );
	int operator==( const ulong& other );
	operator ulong(){ return datl; }
};

#else
#include <stdint.h>
typedef uint64_t llong;

//typedef long long int llong;
#endif


#define _LLONG_H_
#endif
/*** EOF ***/

