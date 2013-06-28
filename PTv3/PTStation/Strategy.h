#pragma once

#include "Trigger.h"
#include "entity/message.pb.h"

class CStrategy
{
public:
	CStrategy(const entity::StrategyItem& strategyItem);
	virtual ~CStrategy(void);

protected:
	vector<TriggerPtr> m_triggers;
	entity::StrategyType m_type;
	bool m_running;
	int m_retryTimes;
};

typedef boost::shared_ptr<CStrategy> StrategyPtr;

