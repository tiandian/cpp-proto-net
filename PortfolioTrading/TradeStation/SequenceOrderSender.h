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

class CSequenceOrderSender
{
public:
	CSequenceOrderSender(
		const string& mlOrderId,
		InputOrderVectorPtr pInputOrdVec, 
		CTradeAgent* pTdAgent,
		std::map<std::string, std::string>* pendingMap);
	~CSequenceOrderSender(void);

	void Start();

	void PreferredOrderDone();

private:
	void SendingProc();

	InputOrderVectorPtr m_inputOrderVec;
	CTradeAgent* m_tradeAgent;
	std::map<std::string, std::string>* m_pendingLegOrderMap;

	boost::thread m_th;
	boost::condition_variable m_preferDoneCond;
	boost::mutex m_mut;

	bool m_preferOrderCompleted;

	std::string m_mlOrderId;
};

