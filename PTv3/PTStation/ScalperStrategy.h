#pragma once

#include "Strategy.h"

class CScalperStrategy : public CStrategy
{
public:
	CScalperStrategy(const entity::StrategyItem& strategyItem);
	~CScalperStrategy(void);

private:
	void CreateTriggers(const entity::StrategyItem& strategyItem);

	double m_priceTick;
	entity::DirectionDepends m_caseLE2Tick;
	entity::DirectionDepends m_caseLE3Tick;
	entity::DirectionDepends m_caseGE4Tick;
	entity::DirectionDepends m_caseNoChange;
	int m_openTimeout;
	entity::StopLossCloseMethods m_stopLossStrategy;
};
