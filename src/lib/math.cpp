#include "math.h"


/*
double Math::catmull
(
    double p0,
    double p1,
    double p2,
    double p3,
    double t
)
{
	double v0 = ( p2 - p0 ) * 0.5;
	double v1 = ( p3 - p1 ) * 0.5;
	double t2 = t * t;
	double t3 = t * t2;
	return
    ( 2 * p1 - 2 * p2 + v0 + v1 ) *
    t3 + ( - 3 * p1 + 3 * p2 - 2 * v0 - v1 ) *
    t2 + v0 * t + p1;
}
*/
