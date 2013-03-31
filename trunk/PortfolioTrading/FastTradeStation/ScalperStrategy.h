#pragma once
#include "DiffStrategy.h"
#include "entity/trade.pb.h"

class CScalperStrategy : public CDiffStrategy
{
public:
	CScalperStrategy(void);
	~CScalperStrategy(void);

	void SetOpenPosiCond(COMPARE_OP op, double targetVal)
	{
		m_openPosiChecker.Comparsion(op);
		m_openPosiChecker.SetTargetVal(targetVal);
	}

	void ApplySettings(const std::string& settingData);

	void Test();

	virtual bool IsAutoOpen(){ return true; }
	virtual bool IsStopGain(){ return false; }
	virtual bool IsStopLoss(){ return false; }

	bool IsAutoTracking(){ return true; }
	bool EnablePrefer(){ return true; }

	double PriceTick(){ return m_priceTick; }
	entity::StopLossCloseMethods CloseMethod() { return m_closeStrategy; }

	virtual int RetryTimes(){ return m_maxRetryTimes; }
	virtual int OpenTimeout() { return m_openTimeout; }

protected:
	CConditionChecker<double>& GetOpenPosiCond(){ return m_openPosiChecker; }
	CConditionChecker<double>& GetStopGainCond(){ return m_stopGainChecker; }
	CConditionChecker<double>& GetStopLossCond(){ return m_stopLossChecker; }

	void DoOpenPostion();

	POSI_OPER NextOperation(POSI_OPER oper);

private:
	CValueChecker m_openPosiChecker;
	CNonUseChecker m_stopGainChecker;
	CNonUseChecker m_stopLossChecker;

	trade::PosiDirectionType GetTradeDirection();

	double m_priceTick;
	entity::DirectionDepends m_CaseLE2Tick;
	entity::DirectionDepends m_CaseLE3Tick;
	entity::DirectionDepends m_CaseGE4Tick;
	entity::DirectionDepends m_CaseNoChange;

	entity::StopLossCloseMethods m_closeStrategy;

	double m_prevAsk;
	double m_Ask;
	int m_AskSize;
	double m_prevBid;
	double m_Bid;
	int m_BidSize;

	int m_maxRetryTimes;
	int m_openTimeout;
};

