#pragma once

#include "MarketAgentCallback.h"
#include "BufferRunner.h"
#include "OperationRecordData.h"
#include "InputOrder.h"
#include "QuoteCache.h"
#include "ConditionChecker.h"

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>

class CReturnOrder;
class CTrade;

namespace OP{

	enum LONG_SHORT_FLAG
	{
		SHORT = 0,
		LONG = 1
	};

	enum OPEN_CLOSE_FLAG
	{
		OPEN = 0,
		CLOSE = 1,
		CLOSE_YESTERDAY = 2
	};

};

class COrderProcessor : public CMarketAgentCallback
{
public:
	COrderProcessor(void);
	~COrderProcessor(void);

	void Initialize();
	void SetSymbol(const std::string& symb);
	void SetQuantity(int qty){ m_orderQty = qty; }
	
	void OpenPosition(int quantity, int longshort);
	void OpenPosition(int quantity, int longshort, double limitprice, int entryReason);
	void ClosePosition();
	void ClosePosition(int longshort, double limitprice, int exitReason);
	void ForceClose(const std::string& symbol, int longshort, int quantity, bool closeYesterday);
	bool CancelOrder();


	void EnableStopGain(bool isEnabled, double gainLimit)
	{ 
		m_stopGain.Enable(isEnabled); 
		m_stopGain.setGainLimit(gainLimit);
	}
	void EnableStopLoss(bool isEnabled, double lossLimit)
	{ 
		m_stopLoss.Enable(isEnabled);
		m_stopLoss.setLossLimit(lossLimit);
	}

	void Start(const BreakOutStrategy* pStrategy);
	void Stop();

	virtual void OnSubscribeCompleted();
	virtual void OnUnsubscribeCompleted();
	virtual void OnQuoteReceived(CQuote* pQuote);

	//////////////////////////////////////////////////////////////////////////
	// Below methods are callbacks for CTradeAgent 

	///登录请求响应
	void OnRspUserLogin(bool succ, std::string& msg, int initOrderRefID);

	///报单录入请求响应
	void OnRspOrderInsert(bool succ, const std::string& orderRef, const std::string& msg);

	///报单操作请求响应
	void OnRspOrderAction(bool succ, const std::string& orderRef, const std::string& msg);

	///报单通知
	void OnRtnOrder(CReturnOrder* order);

	///成交通知
	void OnRtnTrade(CTrade* pTrade);

	///投资者结算结果确认响应
	void OnRspSettlementInfoConfirm(){}

	///请求查询合约响应
	void OnRspQryInstrument(){}

	///请求查询资金账户响应
	void OnRspQryTradingAccount(){}

	///请求查询投资者持仓响应
	void OnRspQryInvestorPosition(){}

	//////////////////////////////////////////////////////////////////////////

private:
	const char* NextOrderRef();

	void ProcessQuote(boost::shared_ptr<CQuote>& pQuote);

	void ForwardQuote(boost::shared_ptr<CQuote>& pQuote);

	void OnPositionClosed();

	void ResetRecord();

	void PublishRecord();

	boost::shared_ptr<CInputOrder> CreateOrder(int quantity, 
		OP::OPEN_CLOSE_FLAG open_close, OP::LONG_SHORT_FLAG long_short, double limitPrice, const char* symbol = NULL);
	
	std::vector<std::string> m_currentSymbols;
	CBufferRunner< boost::shared_ptr<CQuote> > m_bufferRunner;

	int m_orderRefID;
	boost::mutex m_mutOrderRef;
	char ORDER_REF_BUF[13];

	boost::shared_ptr<COperationRecordData> m_currentRecord;
	boost::mutex m_record_mutex;

	CQuoteCache m_latestQuote; 

	COpenPosiCondition m_openCondition;
	CStopGainCondition m_stopGain;
	CStopLossCondition m_stopLoss;
	bool m_isRunning;

	int m_orderQty;

	class SubmitOrderInfo
	{
	public:

		bool Exists() {return !OrderRef.empty(); }

		void Reset()
		{
			Symbol.clear();
			OrderRef.clear();
			ExchangeId.clear();
			OrderSysId.clear();
			UserId.clear();
		}

		std::string Symbol;
		std::string OrderRef;
		std::string ExchangeId;
		std::string OrderSysId;
		std::string UserId;
	};

	SubmitOrderInfo m_pendingOrderInfo;
};

