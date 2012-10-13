#include "StdAfx.h"
#include "Strategy.h"

bool DoubleGreaterEqual(double a, double b)
{
	if(a >= b) return true;
	else
		return b - a < DOUBLE_COMPARSION_PRECISION; 
}

COMPARE_OP ConvertCompareCondition(entity::CompareCondition compareCond)
{
	return (COMPARE_OP)((int)compareCond + 1);
}