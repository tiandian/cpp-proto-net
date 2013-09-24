#include "StdAfx.h"
#include "PriceBarDataProxy.h"
#include "PriceBarDataSource.h"


CPriceBarDataProxy::CPriceBarDataProxy(unsigned int idx, const string& symbol, int precision, CPriceBarDataSource* pDataSource)
	: m_index(idx)
	, m_symbol(symbol)
	, m_precision(precision)
	, m_dataSource(pDataSource)
{
}


CPriceBarDataProxy::~CPriceBarDataProxy(void)
{
	m_dataSource = NULL;
}

const string& CPriceBarDataProxy::DataSourceId()
{
	assert(m_dataSource != NULL);
	return m_dataSource->Id();
}

void CPriceBarDataProxy::InQuote( entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp )
{
	m_dataSource->InQuote(pQuote, timestamp);
}

COHLCRecordSet* CPriceBarDataProxy::GetOHLCRecordSet( boost::chrono::steady_clock::time_point& timestamp )
{
	return m_dataSource->GetRecordSet(timestamp);
}

int CPriceBarDataProxy::GetRecordSetSize()
{
	return m_dataSource->GetRecordSetSize();
}
