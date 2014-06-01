#pragma once

#include "entity/quote.pb.h"

class CPriceBarDataSource;
class COHLCRecordSet;

class CPriceBarDataProxy
{
public:
	CPriceBarDataProxy(unsigned int idx, const string& symbol, int precision, CPriceBarDataSource* pDataSource);
	~CPriceBarDataProxy(void);

	const string& DataSourceId();
	void InQuote(entity::Quote* pQuote, boost::chrono::steady_clock::time_point& timestamp);
	COHLCRecordSet* GetOHLCRecordSet(boost::chrono::steady_clock::time_point& timestamp);
	COHLCRecordSet* GetOHLCRecordSet();
	int GetRecordSetSize();
	unsigned int Index(){ return m_index; }
	const string& Symbol(){ return m_symbol; }
	int Precision() const { return m_precision; }

private:
	string m_symbol;
	int m_precision;
	unsigned int m_index;
	CPriceBarDataSource* m_dataSource;
};

typedef boost::shared_ptr<CPriceBarDataProxy> PriceBarDataProxyPtr;

