

template <class T>
T max( T x, T y ){	return (x < y ) ? y : x ;};
template <class T>
T min( T x, T y ){	return (x < y ) ? x : y ;};

#if 0
class BASICCALCS {
public:
	double max( double a, double b);
	double min( double a, double b);
};
#endif

void TimeToStr(double xt, char *s);

/*** EOF ***/
