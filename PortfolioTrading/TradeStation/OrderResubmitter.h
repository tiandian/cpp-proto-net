#pragma once

#include "../Entity/gen/cpp/trade.pb.h"
#include "../Entity/gen/cpp/quote.pb.h"

#include <string>
#include <map>
#include <boost/smart_ptr.hpp>
#include <boost/thread.hpp>

using namespace std;

enum DoneStatus { NotDone, Filled, Canceled };

class COrderProcessor;

class COrderResubmitter
{
public:
	COrderResubmitter(
		const string& mlOrderId,
		trade::InputOrder* pInputOrd,
		COrderProcessor* pOrderProc,
		int retryTimes);
	~COrderResubmitter(void);

	const std::string& Symbol() const { return m_pendingOrder->instrumentid(); }
	const std::string& OrderRef() const { return m_pendingOrder->orderref(); }

	void UpdateQuote(entity::Quote* pQuote);
	void OnOrderReturn(trade::Order* pOrder);
	int RemainingRetry() { return m_remainingRetry; }
	DoneStatus IsDone(){ return m_isDone; }

	void Start();
	void OrderPending();
private:

	void CancelPending();
	void ModifyOrder(double limitPrice);

	string m_mlOrderId;

	COrderProcessor* m_pOrderProc;

	trade::InputOrder* m_pendingOrder;
	int m_remainingRetry;
	
	boost::mutex m_mut;
	DoneStatus m_isDone;
	bool m_isCanceling;
	bool m_isPending;

	double m_quoteAsk;
	double m_quoteBid;

	boost::thread m_sendingThread;
};

typedef boost::shared_ptr<COrderResubmitter> OrderResubmitterPtr;
typedef map<string, OrderResubmitterPtr> ResubmitterMap;
typedef ResubmitterMap::iterator ResubmitterMapIter;
