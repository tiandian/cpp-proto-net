#pragma once
#include "Strategy.h"

class CChangePositionStrategy : public CStrategy
{
public:
	CChangePositionStrategy(const entity::StrategyItem& strategyItem);
	~CChangePositionStrategy(void);

private:
	void CreateTriggers(const entity::StrategyItem& strategyItem);

	string m_closeLeg;
	entity::PosiDirectionType m_closeLegSide;
};

