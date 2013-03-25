#pragma once

#include "QuoteAgent.h"
#include "TradeAgent.h"
#include "Portfolio.h"
#include "OrderProcessor2.h"
#include "PortfolioOrderHelper.h"
#include "ClientMessagePump.h"
#include "ap_invoke/session.h"

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

using namespace std;

class CClientAgent : public CQuoteAgentCallback
{
public:
	CClientAgent(const string& clntId);
	~CClientAgent(void);

	void SetSession(Session* pSession) 
	{ 
		m_pSession = pSession;
		m_clientMsgPump.Init(pSession);
	}

	bool Detached(){ return m_pSession == NULL; }
	const string& ClientId() { return m_clientId; }
	
	void Add(entity::PortfolioItem* portfolioItem);
	void AddPortfolios( entity::AddPortfolioParam& addPortfParam );
	void Remove(const string& pid);
	void RegQuote(vector<string>& symbols);
	void UnregQuote(vector<string>& symbols);
	
	bool QuerySymbolInfo(const string& symbol, entity::SymbolInfo** ppSymbInfo);
	bool QueryAccountInfo(string* serializedAcctInfo);
	void QueryPositionDetails(const string& symbol);
	void SetPorfPreferredLeg(const entity::ModifyPortfolioPreferredLegParam& preferredLegParam);
	void EnableStrategy(const entity::ModifyRunningStatusParam& runningStatusParam);
	void TurnPortfSwitches(const entity::ModifyPortfolioSwitchParam& switchesParam);
	void ApplyStrategySetting(const entity::ModifyStrategyParam& settings);
	void CancelOrder(const entity::CancelOrderParam& cancelParam);

	void OpenPosition(const string& pid, int quantity);
	void OpenPosition(CPortfolio* portf, int qty, trade::SubmitReason submitReason);
	void ClosePosition( const trade::MultiLegOrder& openMlOrd, const string& legOrdRef, trade::SubmitReason submitReason, string& msg);
	void ClosePosition(const string& portfolioId, int quantity, trade::SubmitReason submitReason);
	void SimpleCloseOrderPosition(const string& portfolioId, trade::SubmitReason submitReason);
	
	void ChangePosition(CPortfolio* portf, const string& closeSymbol, entity::PosiDirectionType existingPosition, int qty, trade::SubmitReason submitReason);
	void QuickScalpe(const string& pid, int quantity);
	void QuickScalpe(CPortfolio* portf, int quantity, trade::PosiDirectionType posiDirection, double precedence);
	boost::tuple<bool, string> ManualCloseOrder(const string& symbol, trade::TradeDirectionType direction, const string& openDate, int quantity);
	void VirtualOpenPosition(const string& pid, int quantity);
	void VirtualClosePosition(const string& pid, int quantity);
	void SetPortfolioQuantity(const string& pid, int qty, int maxQty);

	void OnPortfolioUpdated(entity::PortfolioItem* portfolioItem);
	void OnMultiLegOrderUpdated(trade::MultiLegOrder* order);
	void OnLegOrderUpdated(const string& portfId, const string& mlOrderId, trade::Order* legOrd);
	void OnTradeUpdated(trade::Trade* trade);
	void OnPostionDetailReturned(trade::PositionDetailInfo* posiDetailInfo);
	
	std::string QuoteAddress() const { return m_quoteAddress; }
	boost::tuple<bool, string> QuoteConnect(const std::string& address, const std::string& streamDir);
	void QuoteDisconnect();

	boost::tuple<bool, string> TradeLogin(const string& brokerId, const string& userId, const string& password, const entity::AccountSettings* pAcctSettings);
	void TradeLogout();

	std::string TradeAddress() const { return m_tradeAddress; }
	boost::tuple<bool, string> TradeConnect(const std::string& address, const std::string& streamDir);
	void TradeDisconnect();

	boost::tuple<bool, string> QuoteLogin(const string& brokerId, const string& userId, const string& password);
	void QuoteLogout();

	bool ReachOpenTimesLimit(){ return m_orderProcessor.ReachOpenTimesLimit(); }
	bool ReachCancelTimesLimit() { return m_orderProcessor.ReachCancelTimesLimit(); }

	bool IsConnected(){ return (m_quoteAgent.IsConnected() && m_tradeAgent.IsConnected()); }

	// CQuoteAgentCallback
	virtual void OnSubscribeCompleted(){}
	virtual void OnUnsubscribeCompleted(){}
	virtual void OnQuoteReceived(entity::Quote* pQuote);
	virtual void OnReconnected(){}

private:

	CQuoteAgent			m_quoteAgent;
	CTradeAgent			m_tradeAgent;
	COrderProcessor2	m_orderProcessor;
	CClientMessagePump	m_clientMsgPump;
	boost::shared_ptr<CPortfolio> m_pActPortfolio;

	string m_quoteAddress;
	string m_tradeAddress;
	string m_brokerId;
	string m_userId;

	string m_clientId;

	Session* m_pSession;
};

