#include "StdAfx.h"
#include "TechAnalyStrategy.h"
#include "PriceBarDataSource.h"
#include "TechDataRepo.h"
#include "AvatarClient.h"
#include <assert.h>

CTechDataRepo g_dataRepo;

CTechAnalyStrategy::CTechAnalyStrategy(const entity::StrategyItem& strategyItem, CAvatarClient* pAvatar)
	: CStrategy(strategyItem)
	, m_avatar(pAvatar)
{
}

CTechAnalyStrategy::~CTechAnalyStrategy(void)
{
	if(IsRunning())
		Stop();
}

void CTechAnalyStrategy::Start()
{
	for (vector<HistSrcCfgPtr>::iterator iter = m_histSrcConfigs.begin();
		iter != m_histSrcConfigs.end(); ++iter)
	{
		CPriceBarDataProxy* proxy = g_dataRepo.Register((*iter)->Symbol, (*iter)->Precision, m_avatar->TradingDay());
		if(proxy != NULL)
			m_pDataProxies.push_back(proxy);
	}

	CStrategy::Start();
}

void CTechAnalyStrategy::Stop()
{
	for (vector<CPriceBarDataProxy*>::iterator iter = m_pDataProxies.begin();
		iter != m_pDataProxies.end(); ++iter)
	{
		g_dataRepo.Unregister(*iter);
	}
	m_pDataProxies.clear();

	CStrategy::Stop();
}

void CTechAnalyStrategy::Test( entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp )
{
	CStrategy::Test(pQuote, pPortfolio, timestamp);

	for (vector<CPriceBarDataProxy*>::iterator iter = m_pDataProxies.begin();
		iter != m_pDataProxies.end(); ++iter)
	{
		if((*iter)->Symbol() == pQuote->symbol())
			(*iter)->InQuote(pQuote, timestamp);
	}
}

void CTechAnalyStrategy::Apply( const entity::StrategyItem& strategyItem, bool withTriggers )
{
	for(int i = 0; i < strategyItem.histsources_size(); ++i)
	{
		const entity::HistSourceCfg entityCfg = strategyItem.histsources(i);
		HistSrcCfgPtr histCfg(new CHistSourceCfg(entityCfg.symbol(), entityCfg.precision()));
		m_histSrcConfigs.push_back(histCfg);
	}
}

COHLCRecordSet* CTechAnalyStrategy::GetRecordSet(const string& symbol, unsigned int precision, boost::chrono::steady_clock::time_point& timestamp)
{
  for (vector<CPriceBarDataProxy*>::iterator iter = m_pDataProxies.begin();
		iter != m_pDataProxies.end(); ++iter)
	{
		if((*iter)->Symbol() == symbol &&
			(*iter)->Precision() == precision)
         return (*iter)->GetOHLCRecordSet (timestamp);
   }
   return NULL;
}