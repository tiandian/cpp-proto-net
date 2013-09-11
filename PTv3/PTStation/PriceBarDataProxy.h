#pragma once

#include "entity/quote.pb.h"

class CPriceBarDataSource;
class COHLCRecordSet;

class CPriceBarDataProxy
{
public:
	CPriceBarDataProxy(unsigned int idx, const string& symbol, unsigned int precision, CPriceBarDataSource* pDataSource);
	~CPriceBarDataProxy(void);

	const string& DataSourceId();
	void InQuote(entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp);
	COHLCRecordSet* GetOHLCRecordSet(boost::chrono::steady_clock::time_point& timestamp);
	unsigned int Index(){ return m_index; }
	const string& Symbol(){ return m_symbol; }
	unsigned int Precision(){ return m_precision; }

private:
	string m_symbol;
	unsigned int m_precision;
	unsigned int m_index;
	CPriceBarDataSource* m_dataSource;
};

typedef boost::shared_ptr<CPriceBarDataProxy> PriceBarDataProxyPtr;

