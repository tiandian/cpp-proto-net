#pragma once
#include "Strategy.h"

class CArbitrageStrategy : public CStrategy
{
public:
	CArbitrageStrategy(const entity::StrategyItem& strategyItem);
	~CArbitrageStrategy(void);

private:
	void CreateTriggers(const entity::StrategyItem& strategyItem);

	entity::PosiDirectionType m_side;
};

