
//
//
//	 pulser memory caliculation
//
//	for linux, minGW, armadillo
//

#include "llong.h"
#include "pulmem.h"
#include <string.h>


PULDATC::~PULDATC()
{

}

PULDATC::PULDATC( PULDATU *a)
{
	u_max = 0;
	pulserFreq = 10000000ul;
	databuf = a;
}
PULDATC::PULDATC()
{
	u_max = 0;
	base = 0;
	pulserFreq = 10000000ul;
}





PULDATU* PULDATC::dat( uint32_t pt )
{
	datp = databuf;
	return &datp[pt];
}


uint32_t PULDATC::d32( uint32_t pt )
{
	return dat(pt)->d[base];
}

uint32_t PULDATC::t( uint32_t pt )
{
	return dat(pt)->t;
}

uint32_t PULDATC::end( uint32_t pt )
{
	while( 0xffffffff != t(pt) ){
		pt++;
	}
	return pt;
}

uint32_t PULDATC::dt( llong tim, uint32_t& diff )
{
	llong a;
	uint32_t pt = 0, ta;
	diff = 0; // 960122	(蛇足)本来いりません。
	for( pt = 0, a = 0 ; ; pt++){
		ta = t(pt);
		if( 0xffffffff == ta ) return pt;
		if( 0xff000000 == (0xff000000 & ta) ) continue;
		a += ta;
		if( tim <= a ){
			diff = (uint32_t)(a-tim);
			return pt;
		}
	}
}



void PULDATC::del( uint32_t pt )
{		
	PULDATU tmp;
	while( t(pt) != 0xffffffff ){
		tmp = *(dat(pt+1));	 // セグメントをまたいだ転送を行うためtmpを使用する
		*(dat(pt)) = tmp;
		pt++;
	}
}


uint32_t PULDATC::edg( llong ddd )
{
	uint32_t pt, diff, a;
	if( ddd == 0ull ) return 0L;
	pt = dt( ddd, diff );
	if( diff == 0l ){
		return pt+1;
	}
	ins( pt );
	a = t(pt);
	for( int i = 0 ; i < PULDAT_DL ; i++){//FIX20150421
		dat( pt+1 )->d[i] = dat(pt)->d[i];//FIX20150421
	}//FIX20150421
	dat( pt   )->t = a - diff;
	dat( pt+1 )->t = diff;
	return pt+1;
}


void PULDATC::ins( uint32_t pt )
{
	uint32_t a;
	PULDATU tmp;
	a = end( pt );
	while( (long)pt <= (long)a ){
		tmp = *dat(a);	// セグメントをまたいだ転送を行うためtmpを使用する
		*dat(a+1) = tmp;
		a--;
	}
}



void PULDATC::clr( uint16_t leng )
{
	int i, j;
	if( (u_max-1)<=leng ) leng = u_max;
	for( i = 0 ; i < leng ; i++){
		dat(i)->t = 0xfefffffful;
		for( j = 0 ; j < PULDAT_DL ; j++) dat(i)->d[j] = 0;
	}
	dat(leng)->t = 0xfffffffful;
	for( j = 0 ; j < PULDAT_DL ; j++) dat(i)->d[j] = 0;
	return;
}

void PULDATC::com( llong tim, uint32_t tt, uint16_t dd )
{
	uint32_t pt;
	tt |= 0xff000000ul;
	pt = edg( tim );
	if( 0xff000000 != (t(pt) & 0xff000000)){
		ins( pt );
	}
	dat(pt)->t = tt;
	dat(pt)->d[base] = dd;
}


void PULDATC::lvl( llong sttim, llong edtim, uint32_t mask, uint32_t pat)
{
	uint32_t i, stpt, edpt;
	uint32_t a;
	stpt = edg( sttim );
	edpt = edg( edtim );
	for( i = stpt; i < edpt ; i++){
		a =  mask & d32(i);
		dat(i)->d[base] =  (a | pat);
	}
}

void PULDATC::seclong(llong &ans, long sec, long usec )
{
	llong s, a;
	s = sec;
	ans = usec;
	a = pulserFreq;
	ans += s * a;
}

void PULDATC::dbllong( llong &ans, double t )
{
	uint32_t sec;
	uint32_t usec;
	double fx, tt;
	if( t <= 0.0 ){	//FIX20121214
		ans = 0;
		return;
	}
	sec = (uint32_t)t;
	fx = t - (double)sec;
	tt = fx * (double)pulserFreq;
	usec = (uint32_t)(tt+0.5);	/* BORLAND Bug 回避  1e-6 x 10e6 != 10 */
	seclong( ans, sec, usec );
}


void PULDATC::lvl( // st:64bit, ed:64bit
	uint32_t stH,	uint32_t stL,
	uint32_t edH,	uint32_t edL,
	uint32_t mask,
	uint32_t pat
)
{
	uint64_t st, ed;
	st = ((uint64_t)stH<<32ull)|((uint64_t)stL);
	ed = ((uint64_t)edH<<32ull)|((uint64_t)edL);
	lvl( st, ed, mask, pat  );




}

void PULDATC::lvl( double st, double ed, uint32_t mask, uint32_t pat)
{
	llong a, b;
	dbllong(a, st );
	dbllong(b, ed );
	lvl( a, b, mask, pat );
}

void PULDATC::com( long sec, long usec, uint32_t t, uint16_t d)
{
	llong a;
	seclong(a, sec, usec );
	com( a, t, d );
}

void PULDATC::com( double s, uint32_t t, uint16_t d)
{
	llong a;
	dbllong(a, s );
	com( a, t, d );
}


void PULDATC::clearpulm()
{
	int i;
	for( i = 0 ; i < u_max ; i++){
		memset( &databuf[i], 0, sizeof databuf[0] );
	}
	databuf[u_max-1].t = 0xfffffffful;
}

/*** EOF ***/
