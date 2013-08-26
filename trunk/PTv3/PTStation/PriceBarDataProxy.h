#pragma once

#include "entity/quote.pb.h"

class CPriceBarDataSource;
class CTaIndicatorSet;

class CPriceBarDataProxy
{
public:
	CPriceBarDataProxy(unsigned int idx, CPriceBarDataSource* pDataSource);
	~CPriceBarDataProxy(void);

	const string& DataSourceId();
	void InQuote(entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp);
	CTaIndicatorSet* GetTaIndicatorSet(boost::chrono::steady_clock::time_point& timestamp);
	unsigned int Index(){ return m_index; }

private:
	
	unsigned int m_index;

	CPriceBarDataSource* m_dataSource;
};

typedef boost::shared_ptr<CPriceBarDataProxy> PriceBarDataProxyPtr;

