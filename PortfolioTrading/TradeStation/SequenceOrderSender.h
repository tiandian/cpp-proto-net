#pragma once

#include "../Entity/gen/cpp/trade.pb.h"
#include "TradeAgent.h"

#include <string>
#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

typedef std::vector<boost::shared_ptr<trade::InputOrder>> InputOrderVector;
typedef InputOrderVector* InputOrderVectorPtr;

typedef boost::function<void(trade::InputOrder*, const std::string&)> SubmitOrderFunc; 

class CSequenceOrderSender
{
public:
	CSequenceOrderSender(
		const string& mlOrderId,
		InputOrderVectorPtr pInputOrdVec, 
		SubmitOrderFunc submitOrdFunc);
	~CSequenceOrderSender(void);

	void Start();

	void OrderDone(bool success);

private:
	void SendingProc();

	InputOrderVectorPtr m_inputOrderVec;
	SubmitOrderFunc m_submitOrderFunc;

	boost::thread m_th;
	boost::condition_variable m_preferDoneCond;
	boost::mutex m_mut;

	bool m_preferOrderCompleted;
	bool m_preferOrderCompletionSuccess;

	std::string m_mlOrderId;
};

