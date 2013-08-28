#include "StdAfx.h"
#include "TechAnalyStrategy.h"
#include "PriceBarDataSource.h"
#include "TechDataRepo.h"
#include <assert.h>

CTechDataRepo g_dataRepo;

CTechAnalyStrategy::CTechAnalyStrategy(const entity::StrategyItem& strategyItem)
	: CStrategy(strategyItem)
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
		CPriceBarDataProxy* proxy = g_dataRepo.Register((*iter)->Symbol, (*iter)->Precision);
		if(proxy != NULL)
			m_pDataProxies.push_back(proxy);
	}
	// TODO will replace below parameters
	/*m_pDataProxy = g_dataRepo.Register("Symbol", 60);
	assert(m_pDataProxy != NULL);*/
	//m_dataSource.Init(m_precision);

	//m_priceBarGen.Init(m_precision);

	// read history data and have data source ready
	//m_histDataReader.Read(m_dataSource.RecordSet());

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
	//m_pDataProxy->InQuote(pQuote, timestamp);
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

