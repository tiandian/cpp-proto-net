#pragma once

#include "Strategy.h"

class CPriceBarDataProxy;
class CAvatarClient;
class COHLCRecordSet;

class CHistSourceCfg
{
public:
	CHistSourceCfg(const string& symb, unsigned int precision)
		: Symbol(symb), Precision(precision)
	{}

	string Symbol;
	unsigned int Precision;
};

typedef boost::shared_ptr<CHistSourceCfg> HistSrcCfgPtr;

class CTechAnalyStrategy : public CStrategy
{
public:
	CTechAnalyStrategy(const entity::StrategyItem& strategyItem, CAvatarClient* pAvatar);
	virtual ~CTechAnalyStrategy(void);

	virtual void Apply(const entity::StrategyItem& strategyItem, bool withTriggers);
	virtual void Test(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);

	void Start();
	void Stop();
   
  COHLCRecordSet* GetRecordSet(const string& symbol, unsigned int precision, boost::chrono::steady_clock::time_point& timestamp);  

private:
	vector<HistSrcCfgPtr> m_histSrcConfigs;
	vector<CPriceBarDataProxy*> m_pDataProxies;

	CAvatarClient* m_avatar;
};

