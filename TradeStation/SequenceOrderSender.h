#pragma once

#include "../Entity/gen/cpp/trade.pb.h"
#include "TradeAgent.h"
#include "OrderResubmitter.h"

#include <string>
#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>

typedef std::vector<boost::shared_ptr<trade::InputOrder>> InputOrderVector;
typedef InputOrderVector* InputOrderVectorPtr;

typedef boost::function<void(const string&, COrderResubmitter*, bool)> ChangeSubmitterFunc;

class COrderProcessor;

class CSequenceOrderSender
{
public:
	CSequenceOrderSender(
		const string& portfolioId,
		const string& mlOrderId,
		InputOrderVectorPtr pInputOrdVec, 
		COrderProcessor* orderProc, int retryTimes);
	~CSequenceOrderSender(void);

	void Start();

	bool CheckOrderStatus(trade::Order* pOrder);
	void OrderPending();
	void OrderInsertFailed();

private:
	void SendOrder(int ordIdx);

	OrderResubmitterPtr m_workingResubmitter;

	InputOrderVectorPtr m_inputOrderVec;
	int m_sendingOrderIndex;
	COrderProcessor* m_orderProc;

	int m_retryTimes;
	bool m_preferOrderCompleted;
	bool m_preferOrderCompletionSuccess;

	std::string m_mlOrderId;
	std::string m_portfolioId;
};

