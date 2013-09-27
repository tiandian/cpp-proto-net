#pragma once

#include "TradeAgent.h"
#include "QuoteAgent.h"
#include "PortfolioManager.h"
#include "QuoteRepositry.h"
#include "OrderProcessor.h"
#include "entity/message.pb.h"

#include <boost/date_time.hpp>

class CAvatarClient : public LogicalConnection
{
public:
	CAvatarClient(const string& sessionId);
	~CAvatarClient(void);

	virtual const char* getKey(){ return m_sessionId.c_str(); }

	const string& Pseudo() const { return m_investorId; }
	void Pseudo(const string& val) { m_investorId = val; }

	boost::tuple<bool, string> TradeLogin(const string& address, const string& brokerId, const string& investorId, const string& password);
	void TradeLogout();
	boost::tuple<bool, string> QuoteLogin(const string& address, const string& brokerId, const string& investorId, const string& password);
	void QuoteLogout();

	CPortfolioManager& PortfolioManager(){ return m_portfolioMgr; }
	COrderProcessor& OrderProcessor(){ return m_orderProcessor; }

	void PublishPortfolioUpdate(const entity::PortfolioUpdateItem& portfolioUpdate);
	void PublishMultiLegOrderUpdate(trade::MultiLegOrder* pOrder);
	void PublishLegOrderUpdate(const string& portfId, const string& mlOrderId, trade::Order* legOrd);
	void PublishTradeUpdate(trade::Trade* pTrade);
	void PublishPositionDetail(trade::PositionDetailInfo* pPosiDetailInfo);

	const boost::gregorian::date& TradingDay(){ return m_tradeAgent.TradingDay(); }

	bool m_destroyed;

private:
	string				m_sessionId;
	string				m_investorId;


	CTradeAgent			m_tradeAgent;
	CQuoteAgent			m_quoteAgent;
	CQuoteRepositry		m_quoteRepositry;
	CPortfolioManager	m_portfolioMgr;
	COrderProcessor		m_orderProcessor;
	
	bool m_tradeLogged;
	bool m_quoteLogged;

};

