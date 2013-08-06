#include "StdAfx.h"
#include "PriceBarDataSource.h"


CPriceBarDataSource::CPriceBarDataSource(void)
{
}


CPriceBarDataSource::~CPriceBarDataSource(void)
{
}

void CPriceBarDataSource::Init( int precision )
{
	m_recordSet = OHLCRecordSetPtr(new COHLCRecordSet(precision));
}
