#include "StdAfx.h"
#include "DoubleCompare.h"

bool DoubleGreaterEqual( double a, double b )
{
	if(a >= b) return true;
	else
		return b - a < DOUBLE_PRECISION;
}

bool DoubleLessEqual( double a, double b )
{
	return DoubleGreaterEqual(b, a);
}

bool DoubleEqual( double a, double b )
{
	double diff = a - b;
	return diff < DOUBLE_PRECISION && diff > -DOUBLE_PRECISION;
}
