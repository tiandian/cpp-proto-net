#pragma once

#include "../Entity/gen/cpp/message.pb.h"
#include "ClientAgent.h"
#include "Portfolio.h"
#include "globalmembers.h"

#include <string>
#include <boost/format.hpp>

#define DOUBLE_COMPARSION_PRECISION (0.01)

enum COMPARE_OP
{
	NOT_COMPARE,
	GREATER_THAN,
	GREATER_EQUAL_THAN,
	LESS_THAN,
	LESS_EQUAL_THAN
};

enum POSI_OPER
{
	DO_NOTHING,
	OPEN_POSI, 
	CLOSE_POSI
};

bool DoubleGreaterEqual(double a, double b);
COMPARE_OP ConvertCompareCondition(entity::CompareCondition compareCond);
const char* StrategyOpertaionText(POSI_OPER oper);
POSI_OPER NextOperation(POSI_OPER oper);

template <typename T>
class CConditionChecker
{
public:
	virtual ~CConditionChecker(){}

	virtual bool Test(T value) = 0;
};

template <typename T>
class CStrategy
{
public:
	CStrategy(void):
		m_isRunning(false),
		m_isAutoOpen(false),
		m_isStopGain(false),
		m_isStopLoss(false),
		m_isAutoTracking(false),
		m_enablePrefer(false)
	{
	}
	virtual ~CStrategy(void){}

	void Start()
	{
		m_isRunning = true; 
	}
	void Stop()
	{
		m_isRunning = false; 
	}
	bool IsRunning() { return m_isRunning; }

	bool IsAutoOpen(){ return  m_isAutoOpen; }
	void SetAutoOpen(bool flag){ m_isAutoOpen = flag; }

	bool IsStopGain(){ return m_isStopGain; }
	void SetStopGain(bool flag) { m_isStopGain = flag; }

	bool IsStopLoss(){ return m_isStopLoss; }
	void SetStopLoss(bool flag) { m_isStopLoss = flag; }

	bool IsAutoTracking(){ return m_isAutoTracking; }
	void SetAutoTracking(bool flag) { m_isAutoTracking = flag; }

	bool EnablePrefer(){ return m_enablePrefer; }
	void SetEnablePrefer(bool flag) { m_enablePrefer = flag; }

	CClientAgent* Client() const { return m_pClient; }
	void Client(CClientAgent* val) { m_pClient = val; }

	CPortfolio* Portfolio() const { return m_pPortfolio; }
	void Portfolio(CPortfolio* val) { m_pPortfolio = val; }

	void Test(T valueToTest, POSI_OPER testFor = DO_NOTHING)
	{
		if(m_isRunning)
		{
			if(testFor == DO_NOTHING || testFor == OPEN_POSI)
			{
				if(!m_pPortfolio->PositionReachLimit() && m_isAutoOpen)
				{
					logger.Info(boost::str(boost::format("Test for %s") 
						% StrategyOpertaionText(OPEN_POSI)));

					bool succ = GetOpenPosiCond().Test(valueToTest);
					if(succ)
					{
						DoOpenPostion();
					}
				}
			}

			if(testFor == DO_NOTHING || testFor == CLOSE_POSI)
			{
				if(m_pPortfolio->HasPosition())
				{
					if(m_isStopGain)
					{
						logger.Info(boost::str(boost::format("Test for %s") 
							% "Stop Gain"));

						bool succ = GetStopGainCond().Test(valueToTest);
						if(succ)
						{
							DoStopGain();
						}
					}
					if(m_isStopLoss)
					{
						logger.Info(boost::str(boost::format("Test for %s") 
							% "Stop Loss"));

						bool succ = GetStopLossCond().Test(valueToTest);
						if(succ)
						{
							DoStopLoss();
						}
					}
				}
			}
		}
	}

	virtual void ApplySettings(const std::string& settingData) = 0;

protected:
	virtual CConditionChecker<T>& GetOpenPosiCond() = 0;
	virtual CConditionChecker<T>& GetStopGainCond() = 0;
	virtual CConditionChecker<T>& GetStopLossCond() = 0;

	virtual void DoOpenPostion(){}
	virtual void DoStopGain(){}
	virtual void DoStopLoss(){}

	virtual POSI_OPER NextOperation(POSI_OPER oper)
	{
		return DO_NOTHING;
	}

protected:
	bool m_isRunning;
	bool m_isAutoOpen;
	bool m_isStopGain;
	bool m_isStopLoss;
	bool m_isAutoTracking;
	bool m_enablePrefer;

	CClientAgent* m_pClient;
	CPortfolio* m_pPortfolio;

};

