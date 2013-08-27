#include "StdAfx.h"
#include "OHLCRecordSet.h"


COHLCRecordSet::COHLCRecordSet(int m_precision)
{
}


COHLCRecordSet::~COHLCRecordSet(void)
{
}

void COHLCRecordSet::Set( int barIdx, double open, double high, double low, double close )
{
	OpenSeries[barIdx] = open;
	HighSeries[barIdx] = high;
	LowSeries[barIdx] = low;
	CloseSeries[barIdx] = close;
}
