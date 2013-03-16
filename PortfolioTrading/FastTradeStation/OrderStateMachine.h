#pragma once

#include "OrderState.h"

#include <string>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

using namespace std;
using namespace op2;

class COrderPlacer : public CStateOwner
{
public:
	COrderPlacer(){}
	virtual ~COrderPlacer(){}

	virtual const string& Id() = 0;
	virtual void Do(){}
	virtual bool AutoDispose() { return true; }
};

typedef boost::shared_ptr<COrderPlacer> OrderPlacerPtr;
typedef map<string, OrderPlacerPtr> OrderPlacerMap;
typedef OrderPlacerMap::iterator OrderPlacerMapIter;

typedef boost::shared_ptr<COrderState> OrderStatePtr;
typedef map<ORDER_STATE, OrderStatePtr> OrderStateMap;
typedef OrderStateMap::iterator OrderStateMapIter;

class COrderStateMachine
{
public:
	COrderStateMachine(void)
	{}
	virtual ~COrderStateMachine(void){}

	void AddPlacer(const OrderPlacerPtr& placer)
	{
		boost::recursive_mutex::scoped_lock lock(m_mut);
		m_orderPlacerMap.insert(make_pair(placer->Id(), placer));
	}
	void RemovePlacer(const string& placerId)
	{
		boost::recursive_mutex::scoped_lock lock(m_mut);
		m_orderPlacerMap.erase(placerId);
	}

	bool RemovePlacer(const string& placerId, OrderPlacerPtr* pRemovedPlacer)
	{
		boost::recursive_mutex::scoped_lock lock(m_mut);
		OrderPlacerMapIter iter = m_orderPlacerMap.find(placerId);
		if(iter != m_orderPlacerMap.end())
		{
			if(pRemovedPlacer != NULL)
				*pRemovedPlacer = iter->second;
			m_orderPlacerMap.erase(iter);
			return true;
		}
		return false;
	}

	COrderState* GetState(ORDER_STATE stateVal)
	{
		OrderStateMapIter iter = m_orderStates.find(stateVal);
		if(iter != m_orderStates.end())
		{
			return (iter->second).get();
		}

		return NULL;
	}

	virtual void Transition(const string& orderId, COrderEvent& event);

protected:
	
	OrderPlacerMap m_orderPlacerMap;
	boost::recursive_mutex m_mut;

	OrderStateMap m_orderStates;
};

