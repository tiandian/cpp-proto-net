#include "StdAfx.h"
#include "TechAnalyStrategy.h"
#include "PriceBarDataSource.h"
#include "TechDataRepo.h"
#include <assert.h>

CTechDataRepo g_dataRepo;

CTechAnalyStrategy::CTechAnalyStrategy(const entity::StrategyItem& strategyItem)
	: CStrategy(strategyItem)
	, m_precision(60)
	, m_pDataProxy(NULL)
{
}

CTechAnalyStrategy::~CTechAnalyStrategy(void)
{
	m_pDataProxy = NULL;
}

void CTechAnalyStrategy::Start()
{
	// TODO will replace below parameters
	m_pDataProxy = g_dataRepo.Register("Symbol", 60, TA_MACD);
	assert(m_pDataProxy != NULL);
	//m_dataSource.Init(m_precision);

	//m_priceBarGen.Init(m_precision);

	// read history data and have data source ready
	//m_histDataReader.Read(m_dataSource.RecordSet());

	CStrategy::Start();
}

void CTechAnalyStrategy::Stop()
{
	g_dataRepo.Unregister(m_pDataProxy);
	m_pDataProxy = NULL;
	CStrategy::Stop();
}

void CTechAnalyStrategy::Test( entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp )
{
	m_pDataProxy->InQuote(pQuote, timestamp);
}

