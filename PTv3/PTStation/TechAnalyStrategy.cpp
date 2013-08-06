#include "StdAfx.h"
#include "TechAnalyStrategy.h"


CTechAnalyStrategy::CTechAnalyStrategy(const entity::StrategyItem& strategyItem)
	: CStrategy(strategyItem)
	, m_precision(60)
{
}

CTechAnalyStrategy::~CTechAnalyStrategy(void)
{
}

void CTechAnalyStrategy::Start()
{
	m_dataSource.Init(m_precision);

	m_priceBarGen.Init(m_precision);

	// read history data and have data source ready
	m_histDataReader.Read(m_dataSource.RecordSet());

	CStrategy::Start();
}

void CTechAnalyStrategy::Stop()
{
	CStrategy::Stop();
}

void CTechAnalyStrategy::Test( entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp )
{
	m_priceBarGen.Calculate(pQuote);
}

void CTechAnalyStrategy::OnBarChanged( int barIdx, double open, double high, double low, double close )
{
	m_dataSource.RecordSet()->OpenSeries[barIdx] = open;
	m_dataSource.RecordSet()->HighSeries[barIdx] = high;
	m_dataSource.RecordSet()->LowSeries[barIdx] = low;
	m_dataSource.RecordSet()->CloseSeries[barIdx] = close;
}

void CTechAnalyStrategy::OnBarFinalized( int barIdx, double open, double high, double low, double close )
{
	m_histDataWriter.Write(open, high, low, close);
}
