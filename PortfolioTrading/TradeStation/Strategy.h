#pragma once


enum POSI_OPER
{
	DO_NOTHING,
	OPEN_POSI, 
	CLOSE_POSI
};
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
		m_isAutoClose(false),
		m_testingFor(DO_NOTHING)
	{
	}
	virtual ~CStrategy(void){}

	void Start(){ m_isRunning = true; }
	void Stop(){ m_isRunning = false; }
	bool IsRunning() { return m_isRunning; }

	bool IsAutoOpen(){ return  m_isAutoOpen; }
	void SetAutoOpen(bool flag){ m_isAutoOpen = flag; }

	bool IsAutoClose(){ return m_isAutoClose; }
	void SetAutoClose(bool flag) { m_isAutoClose = flag; }

	POSI_OPER Test(T valueToTest)
	{
		if(m_isRunning)
		{
			if(m_testingFor == OPEN_POSI)
			{
				bool succ = GetOpenPosiCond().Test(valueToTest);
				if(succ)
					return m_testingFor;
			}
			else if(m_testingFor == CLOSE_POSI)
			{
				bool succ = GetStopGainCond().Test(valueToTest);
				if(succ)
					return m_testingFor;
				else
				{
					succ = GetStopLossCond().Test(valueToTest);
					if(succ)
						return m_testingFor;
				}
			}
		}

		return DO_NOTHING;
	}

	void TestFor(POSI_OPER posiOperation){ m_testingFor = DO_NOTHING; }

protected:
	virtual CConditionChecker<T>& GetOpenPosiCond() = 0;
	virtual CConditionChecker<T>& GetStopGainCond() = 0;
	virtual CConditionChecker<T>& GetStopLossCond() = 0;

private:
	bool m_isRunning;
	bool m_isAutoOpen;
	bool m_isAutoClose;

	POSI_OPER m_testingFor;
};

