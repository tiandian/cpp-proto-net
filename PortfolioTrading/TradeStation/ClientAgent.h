#pragma once

#include "QuoteAgent.h"
#include "TradeAgent.h"
#include "QuoteAggregator.h"
#include "PortfolioManager.h"
#include "ManualOrderPlacer.h"
#include "OrderProcessor.h"
#include "QuoteListener.h"
#include "PortfolioOrderHelper.h"
#include "../../APInvoke/APInvokeNative/APInvokeNative.h"

#include <string>
#include <vector>

using namespace std;

class CClientAgent : public CQuoteListener
{
public:
	CClientAgent(void);
	~CClientAgent(void);

	void SetSession(Session* pSession) 
	{ 
		m_pSession = pSession; 
		SetClientStatus( m_pSession != NULL );
	}

	void Add(entity::PortfolioItem* portfolioItem);
	void AddPortfolios( entity::AddPortfolioParam& addPortfParam );
	void Remove(const string& pid);
	void RegQuote(vector<string>& symbols);
	
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
	boost::tuple<bool, string> ManualCloseOrder(const string& symbol, trade::TradeDirectionType direction, trade::OffsetFlagType offsetFlag, int quantity);
	void VirtualOpenPosition(const string& pid, int quantity);
	void VirtualClosePosition(const string& pid, int quantity);
	void SetPortfolioQuantity(const string& pid, int qty, int maxQty);

	void OnQuoteRecevied(boost::shared_ptr<entity::Quote>& pQuote);
	void OnPortfolioUpdated(entity::PortfolioItem* portfolioItem);
	void OnMultiLegOrderUpdated(trade::MultiLegOrder* order);
	void OnLegOrderUpdated(const string& portfId, const string& mlOrderId, trade::Order* legOrd);
	void OnTradeUpdated(trade::Trade* trade);
	void OnPostionDetailReturned(trade::PositionDetailInfo* posiDetailInfo);
	
	std::string QuoteAddress() const { return m_quoteAddress; }
	boost::tuple<bool, string> QuoteConnect(const std::string& address, const std::string& streamDir);
	void QuoteDisconnect();

	boost::tuple<bool, string> TradeLogin(const string& brokerId, const string& userId, const string& password);
	void TradeLogout();

	std::string TradeAddress() const { return m_tradeAddress; }
	boost::tuple<bool, string> TradeConnect(const std::string& address, const std::string& streamDir);
	void TradeDisconnect();

	boost::tuple<bool, string> QuoteLogin(const string& brokerId, const string& userId, const string& password);
	void QuoteLogout();

private:

	void SetClientStatus(bool connected) { m_clientConnected = connected; }


	CQuoteAgent			m_quoteAgent;
	CQuoteAggregator	m_quoteAggregator;
	CTradeAgent			m_tradeAgent;
	CPortfolioManager	m_portfolioMgr;
	COrderProcessor		m_orderProcessor;

	string m_quoteAddress;
	string m_tradeAddress;
	string m_brokerId;
	string m_userId;

	Session* m_pSession;

	bool m_clientConnected;
};

