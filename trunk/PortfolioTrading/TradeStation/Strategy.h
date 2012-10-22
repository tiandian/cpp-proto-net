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
		m_testingFor(DO_NOTHING)
	{
	}
	virtual ~CStrategy(void){}

	void Start()
	{
		if(m_isAutoOpen) 
			m_testingFor = OPEN_POSI;
		else
			m_testingFor = CLOSE_POSI;

		m_isRunning = true; 
	}
	void Stop()
	{
		m_testingFor = DO_NOTHING;
		m_isRunning = false; 
	}
	bool IsRunning() { return m_isRunning; }

	bool IsAutoOpen(){ return  m_isAutoOpen; }
	void SetAutoOpen(bool flag){ m_isAutoOpen = flag; }

	bool IsStopGain(){ return m_isStopGain; }
	void SetStopGain(bool flag) { m_isStopGain = flag; }

	bool IsStopLoss(){ return m_isStopLoss; }
	void SetStopLoss(bool flag) { m_isStopLoss = flag; }

	CClientAgent* Client() const { return m_pClient; }
	void Client(CClientAgent* val) { m_pClient = val; }

	CPortfolio* Portfolio() const { return m_pPortfolio; }
	void Portfolio(CPortfolio* val) { m_pPortfolio = val; }

	void Test(T valueToTest)
	{
		if(m_isRunning)
		{				
			logger.Info(boost::str(boost::format("Test for %s") 
						% StrategyOpertaionText(m_testingFor)));

			if(m_testingFor == OPEN_POSI && m_isAutoOpen)
			{
				bool succ = GetOpenPosiCond().Test(valueToTest);
				if(succ)
				{
					DoOpenPostion();
					TestFor(NextOperation(OPEN_POSI));
				}
			}
			else if(m_testingFor == CLOSE_POSI)
			{
				if(m_isStopGain)
				{
					bool succ = GetStopGainCond().Test(valueToTest);
					if(succ)
					{
						DoStopGain();
						TestFor(NextOperation(CLOSE_POSI));
					}
				}
				else if(m_isStopLoss)
				{
					bool succ = GetStopLossCond().Test(valueToTest);
					if(succ)
					{
						DoStopLoss();
						TestFor(NextOperation(CLOSE_POSI));
					}
				}
			}
		}
	}

	void TestFor(POSI_OPER posiOperation)
	{ 
		m_testingFor = posiOperation;
		if(m_testingFor == DO_NOTHING)
			m_pPortfolio->EnableStrategy(false);	
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

	POSI_OPER m_testingFor;

	CClientAgent* m_pClient;
	CPortfolio* m_pPortfolio;

};

