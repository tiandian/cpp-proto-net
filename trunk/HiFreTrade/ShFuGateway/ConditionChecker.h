#pragma once

#include <queue>

class CConditionChecker
{
public:
	CConditionChecker(){}
	virtual ~CConditionChecker(){}

	virtual bool Check(double last, int* offsetFlag) = 0;
};

class COpenPosiCondition : public CConditionChecker
{
public:
	COpenPosiCondition(void);
	~COpenPosiCondition(void);

	bool Check(double last, int* offsetFlag){ return false; }

private:
	std::queue<double> m_quoteQueue;
};

class CStopGainCondition : public CConditionChecker
{
public:
	CStopGainCondition(void){}
	~CStopGainCondition(void){}

	bool Check(double last, int* offsetFlag){ return false; }
};

class CStopLossCondition : public CConditionChecker
{
public:
	CStopLossCondition(void){}
	~CStopLossCondition(void){}

	bool Check(double last, int* offsetFlag){ return false; }
};