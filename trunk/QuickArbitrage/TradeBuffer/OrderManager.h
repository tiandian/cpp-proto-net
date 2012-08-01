#pragma once

#include "QuoteListener.h"
#include "RemoteClient.h"
#include "TradeAgent.h"
#include "Portfolio.h"
#include "OrderRepository.h"
#include "TransactionDB.h"

#include <string>
#include <vector>
#include <map>
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
	bool Portfolio_CancelLegOrder(const boost::uuids::uuid& pid, int legIdx = -1);

	CPortfolio* GetPortfolio(const boost::uuids::uuid& pid);
	CPortfolio* GetPortfolio(int portIdx);

	void Buy()
	{
		m_tradeAgent.Buy();
	}

	virtual void OnQuoteRecevied(boost::shared_ptr<CTP::Quote>& pQuote);

	//////////////////////////////////////////////////////////////////////////
	// Below methods are callbacks for CTradeAgent 

	///��¼������Ӧ
	void OnRspUserLogin(bool succ, std::string& msg, int initOrderRefID);

	///����¼��������Ӧ
	void OnRspOrderInsert(bool succ, const std::string& orderRef, const std::string& msg);

	///��������������Ӧ
	void OnRspOrderAction(bool succ, const std::string& orderRef, const std::string& msg);

	///����֪ͨ
	void OnRtnOrder(protoc::Order* order);

	///�ɽ�֪ͨ
	void OnRtnTrade(protoc::Trade* pTrade);

	///Ͷ���߽�����ȷ����Ӧ
	void OnRspSettlementInfoConfirm(){}

	///�����ѯ��Լ��Ӧ
	void OnRspQryInstrument(){}

	///�����ѯ�ʽ��˻���Ӧ
	void OnRspQryTradingAccount(){}

	///�����ѯͶ���ֲ߳���Ӧ
	void OnRspQryInvestorPosition(){}
	
	//////////////////////////////////////////////////////////////////////////
private:

	const char* NextOrderRef();

	PortfolioVecIter FindPortfolio(const boost::uuids::uuid& pid);

	boost::shared_ptr<protoc::InputOrder> CreateInputOrderByLeg(CLeg* leg);
	boost::shared_ptr<protoc::InputOrderAction> CreateCancelActionByLeg(CLeg* leg);

	void AddPortfolioListenQuote(CPortfolio* portfolio, bool submit = true);
	void RemovePortfolioListenQuote(CPortfolio* portfolio);

	void AddPortfolio(const boost::shared_ptr<CPortfolio>& portfolio, bool submit = true);
	void UpdateListeningQuote();

	CTradeAgent	m_tradeAgent;
	
	ClientBase* m_pClient;

	PortfolioVector m_portfolioVec;
	COrderRepository m_orderRepo;

	int m_orderRefID;
	boost::mutex m_mutOrderRef;
	TThostFtdcOrderRefType ORDER_REF_BUF;

	CTransactionDB m_database;

	typedef std::multimap<std::string, CPortfolio*> SymbolPortfolioMap;
	typedef SymbolPortfolioMap::iterator SymbolPortfolioMapIter;
	SymbolPortfolioMap m_dispatchMap;
	typedef boost::shared_mutex Lock; 
	typedef boost::unique_lock< boost::shared_mutex > WriteLock; 
	typedef boost::shared_lock< boost::shared_mutex >  ReadLock;
	Lock m_lock;
};
