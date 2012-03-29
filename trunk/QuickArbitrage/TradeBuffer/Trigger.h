#pragma once

#include "protobuf_gen/quote.pb.h"

#include <boost/function.hpp>
#include <boost/uuid/uuid.hpp>

typedef boost::function<bool(const boost::uuids::uuid&)> TriggerHandler;

class CTrigger
{
public:
	CTrigger(void);
	virtual ~CTrigger(void);

	bool Test(CTP::Quote* quote);

	bool GetIsTriggered(){ return m_isTriggered; }
	void Reset(){ m_isTriggered = false; }

	bool GetIsActive(){ return m_isActive; }
	void Activate(bool active) { m_isActive = active; }

	void SetHandler(TriggerHandler handler) { m_handler = handler; }

protected:

	virtual bool OnConditionCheck(CTP::Quote* quote) = 0;
	virtual void OnTriggered() = 0;

	void InvokeHandler(const boost::uuids::uuid& pid){ if(m_handler != NULL) m_handler(pid); }

	bool m_isTriggered;
	bool m_isActive;
	TriggerHandler m_handler;

};

