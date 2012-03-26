#pragma once

#include "QuoteListener.h"
#include "RemoteClient.h"
#include "TradeAgent.h"
#include "Portfolio.h"
#include "OrderRepository.h"

#include <string>
#include <vector>
#include <boost/thread.hpp>

typedef std::vector< boost::shared_ptr< CPortfolio > > PortfolioVector;
typedef PortfolioVector::iterator PortfolioVecIter;

typedef std::vector< boost::shared_ptr<protoc::InputOrder> > InputOrderVec;
typedef InputOrderVec::iterator InputOrderVecIter;

class COrderManager : public QuoteListener
{
public:
	COrderManager(void);
	~COrderManager(void);

	void Initialize();

	bool Register(ClientBase* pClient, const std::string& brokerId, const std::string& userId, const std::string& password);
	void Unregister(std::string& brokerId, std::string& userId);

	ClientBase* GetCurrentClient(){ return m_pClient; }
	void SetCurrentClient(ClientBase* pClient) { m_pClient = pClient; }

	void Subscribe(std::vector<std::string>& symbols);
	void UnSubscribe();

	void AddPortfolio(CPortfolio* pPortfolio);

	void RemovePortfolio(const boost::uuids::uuid& pid);

	int GetPortfolioCount() { return m_portfolioVec.size(); }

	const PortfolioVector& GetPortfolios() { return m_portfolioVec; }

	bool Portfolio_OpenPosition(const boost::uuids::uuid& pid);
	bool Portfolio_ClosePosition(const boost::uuids::uuid& pid);

	CPortfolio* GetPortfolio(const boost::uuids::uuid& pid);

	void Buy()
	{
		m_tradeAgent.Buy();
	}

	virtual void OnQuoteRecevied(boost::shared_ptr<CTP::Quote>& pQuote);

	//////////////////////////////////////////////////////////////////////////
	// Below methods are callbacks for CTradeAgent 

	///登录请求响应
	void OnRspUserLogin(bool succ, std::string& msg, int initOrderRefID);

	///报单录入请求响应
	void OnRspOrderInsert(bool succ, const std::string& orderRef, const std::string& msg);

	///报单通知
	void OnRtnOrder(protoc::Order* order);

	///成交通知
	void OnRtnTrade(protoc::Trade* pTrade);

	///投资者结算结果确认响应
	void OnRspSettlementInfoConfirm(){}

	///请求查询合约响应
	void OnRspQryInstrument(){}

	///请求查询资金账户响应
	void OnRspQryTradingAccount(){}

	///请求查询投资者持仓响应
	void OnRspQryInvestorPosition(){}

	

	///报单操作请求响应
	void OnRspOrderAction(){}


	

	
	//////////////////////////////////////////////////////////////////////////
private:

	const char* NextOrderRef();

	PortfolioVecIter FindPortfolio(const boost::uuids::uuid& pid);

	boost::shared_ptr<protoc::InputOrder> CreateInputOrderByLeg(CLeg* leg);

	CTradeAgent	m_tradeAgent;
	
	ClientBase* m_pClient;

	PortfolioVector m_portfolioVec;
	COrderRepository m_orderRepo;

	int m_orderRefID;
	boost::mutex m_mutOrderRef;
	TThostFtdcOrderRefType ORDER_REF_BUF;
};

