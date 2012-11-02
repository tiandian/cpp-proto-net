#pragma once

#include "../Entity/gen/cpp/trade.pb.h"
#include "../Entity/gen/cpp/quote.pb.h"

#include <string>
#include <map>
#include <boost/smart_ptr.hpp>

using namespace std;

class COrderResubmitter
{
public:
	COrderResubmitter(trade::InputOrder* pInputOrd);
	~COrderResubmitter(void);

	const std::string& Symbol() const { return m_pendingOrder->instrumentid(); }
	const std::string& OrderRef() const { return m_pendingOrder->orderref(); }

	void UpdateQuote(entity::Quote* pQuote){}
	int RemainingRetry() { return m_remainingRetry; }
	bool IsDone(trade::Order* pOrder){ return true; }

private:

	void CancelPending(){}

	trade::InputOrder* m_pendingOrder;
	int m_remainingRetry;
};

typedef boost::shared_ptr<COrderResubmitter> OrderResubmitterPtr;
typedef map<string, OrderResubmitterPtr> ResubmitterMap;
typedef ResubmitterMap::iterator ResubmitterMapIter;
