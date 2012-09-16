#include "StdAfx.h"
#include "ArbitrageStrategy.h"


bool DoubleGreaterEqual(double a, double b)
{
	if(a >= b) return true;
	else
		return b - a < DOUBLE_COMPARSION_PRECISION; 
}

CArbitrageStrategy::CArbitrageStrategy(void)
{
}


CArbitrageStrategy::~CArbitrageStrategy(void)
{
}
