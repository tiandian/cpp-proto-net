#pragma once

#include "entity/quote.pb.h"
#include "entity/message.pb.h"
#include "entity/trade.pb.h"

#include <string>

using namespace std;

class CPortfolioOrderPlacer;

class CScalperStrategy2
{
public:
	CScalperStrategy2(CPortfolioOrderPlacer* pOrdPlacer);
	~CScalperStrategy2(void);

	void Test(entity::Quote* pQuote);
	void ApplySettings( const string& settingData );
	void Start(){ m_isRunning = true; }
	void Stop(){ m_isRunning = false; }

	int RetryTimes(){ return m_maxRetryTimes; }
	int OpenTimeout() { return m_openTimeout; }
	double PriceTick() { return m_priceTick; }
private:

	bool DoubleGreaterEqual(double a, double b)
	{
		if(a >= b) return true;
		else
			return b - a < 0.01; 
	}

	bool DoubleLessEqual(double a, double b)
	{
		return DoubleGreaterEqual(b, a);
	}

	bool DoubleEqual(double a, double b)
	{
		double diff = a - b;
		return diff < 0.01 && diff > -0.01;
	}

	trade::PosiDirectionType GetTradeDirection();
	trade::PosiDirectionType CalcTradeDirection(int askSize, int bidSize, double askDiff, double bidDiff, entity::DirectionDepends dependsOn);

	CPortfolioOrderPlacer* m_pOrdPlacer;
	bool m_isRunning;

	double m_threshold;
	double m_priceTick;
	entity::DirectionDepends m_CaseLE2Tick;
	entity::DirectionDepends m_CaseLE3Tick;
	entity::DirectionDepends m_CaseGE4Tick;
	entity::DirectionDepends m_CaseNoChange;

	double m_prevAsk;
	double m_Ask;
	int m_AskSize;
	double m_prevBid;
	double m_Bid;
	int m_BidSize;

	int m_maxRetryTimes;
	int m_openTimeout;
};

