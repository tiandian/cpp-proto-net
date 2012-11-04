#pragma once

#include "../Entity/gen/cpp/trade.pb.h"
#include "../Entity/gen/cpp/quote.pb.h"

#include <string>
#include <map>
#include <boost/smart_ptr.hpp>
#include <boost/function.hpp>

using namespace std;

typedef boost::function<void(trade::InputOrder*, const string&)> SubmitOrderFunc; 

class COrderProcessor;

class COrderResubmitter
{
public:
	COrderResubmitter(
		const string& mlOrderId,
		trade::InputOrder* pInputOrd,
		COrderProcessor* pOrderProc);
	~COrderResubmitter(void);

	const std::string& Symbol() const { return m_pendingOrder->instrumentid(); }
	const std::string& OrderRef() const { return m_pendingOrder->orderref(); }

	void UpdateQuote(entity::Quote* pQuote);
	void OnOrderReturn(trade::Order* pOrder);
	int RemainingRetry() { return m_remainingRetry; }
	bool IsDone(){ return m_isDone; }

	void Start();

private:

	void CancelPending(trade::Order* pOrder);
	void ModifyOrder(double limitPrice);

	string m_mlOrderId;

	COrderProcessor* m_pOrderProc;

	trade::InputOrder* m_pendingOrder;
	trade::Order* m_lastRtnOrder;
	int m_remainingRetry;

	bool m_isDone;

	double m_quoteAsk;
	double m_quoteBid;
};

typedef boost::shared_ptr<COrderResubmitter> OrderResubmitterPtr;
typedef map<string, OrderResubmitterPtr> ResubmitterMap;
typedef ResubmitterMap::iterator ResubmitterMapIter;
