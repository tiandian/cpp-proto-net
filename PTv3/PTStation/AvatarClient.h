#pragma once

#include "QuoteAgentFacade.h"
#include "PortfolioManager.h"
#include "QuoteRepositry.h"
#include "OrderProcessor.h"
#include "TechDataRepo.h"
#include "entity/message.pb.h"

#ifndef USE_FEMAS_API
#include "TradeAgent.h"
#else
#include "TradeAgent_FM.h"
#endif // !USE_FEMAS_API


#include <boost/date_time.hpp>

class CAvatarClient : public LogicalConnection
{
public:
	CAvatarClient(const string& sessionId);
	~CAvatarClient(void);

	virtual const char* getKey(){ return m_sessionId.c_str(); }

	const string& Pseudo() const { return m_investorId; }
	void Pseudo(const string& val) { m_investorId = val; }

	boost::tuple<bool, string> TradeLogin(const string& address, const string& brokerId, const string& investorId, const string& userId, const string& password);
	void TradeLogout();
	boost::tuple<bool, string> QuoteLogin(const string& address, const string& brokerId, const string& investorId, const string& userId, const string& password);
	void QuoteLogout();

	CPortfolioManager& PortfolioManager(){ return m_portfolioMgr; }
	COrderProcessor& OrderProcessor(){ return m_orderProcessor; }
	CTechDataRepo& TechDataRepo(){ return m_dataRepo; }

	void PublishPortfolioUpdate(const entity::PortfolioUpdateItem& portfolioUpdate);
	void PublishMultiLegOrderUpdate(trade::MultiLegOrder* pOrder);
	void PublishLegOrderUpdate(const string& portfId, const string& mlOrderId, trade::Order* legOrd);
	void PublishTradeUpdate(trade::Trade* pTrade);
	void PublishPositionDetail(trade::PositionDetailInfo* pPosiDetailInfo);

	const boost::gregorian::date& TradingDay(){ return m_tradeAgent.TradingDay(); }

	bool m_destroyed;

private:

	void UnderlyingPushPacket(OutgoingPacket* pPacket)
	{
		if(!IsInactive())
			TryPushPacket(pPacket);
	}

	string				m_sessionId;
	string				m_investorId;
	string				m_userId;

	CTechDataRepo		m_dataRepo;
	CTradeAgent			m_tradeAgent;
	CQuoteRepositry		m_quoteRepositry;
	CQuoteAgentFacade	m_quoteAgent;
	CPortfolioManager	m_portfolioMgr;
	COrderProcessor		m_orderProcessor;
	
	bool m_tradeLogged;
	bool m_quoteLogged;

};

