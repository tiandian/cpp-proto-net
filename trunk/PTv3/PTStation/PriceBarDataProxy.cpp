#include "StdAfx.h"
#include "PriceBarDataProxy.h"
#include "PriceBarDataSource.h"


CPriceBarDataProxy::CPriceBarDataProxy(unsigned int idx, CPriceBarDataSource* pDataSource)
	: m_index(idx)
	, m_dataSource(pDataSource)
{
}


CPriceBarDataProxy::~CPriceBarDataProxy(void)
{
}

const string& CPriceBarDataProxy::DataSourceId()
{
	return m_dataSource != NULL ? m_dataSource->Id() : "";
}

void CPriceBarDataProxy::InQuote( entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp )
{
	m_dataSource->InQuote(pQuote, timestamp);
}

CTaIndicatorSet* CPriceBarDataProxy::GetTaIndicatorSet(boost::chrono::steady_clock::time_point& timestamp)
{
	return m_dataSource->GetTaIndicatorSet(timestamp);
}
