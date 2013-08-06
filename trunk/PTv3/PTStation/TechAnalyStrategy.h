#pragma once

#include "Strategy.h"
#include "PriceBarGen.h"
#include "HistDataWriter.h"
#include "HistDataReader.h"
#include "PriceBarDataSource.h"

class CTechAnalyStrategy : public CStrategy
{
public:
	CTechAnalyStrategy(const entity::StrategyItem& strategyItem);
	virtual ~CTechAnalyStrategy(void);

	virtual void Test(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);

	void Start();
	void Stop();

private:

	void OnBarChanged(int barIdx, double open, double high, double low, double close);
	void OnBarFinalized(int barIdx, double open, double high, double low, double close);

	CPriceBarGen m_priceBarGen;
	CHistDataWriter m_histDataWriter;
	CHistDataReader m_histDataReader;
	CPriceBarDataSource m_dataSource;

	int m_precision;
};

