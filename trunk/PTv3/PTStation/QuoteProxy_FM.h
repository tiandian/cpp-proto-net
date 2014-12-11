#pragma once

#include "FemasAPI/USTPFtdcMduserApi.h"
#include <boost/thread.hpp>

class CMdSpi;
class CQuoteAggregator;

class CQuoteProxy
{
public:
	CQuoteProxy(CQuoteAggregator* pQuoteAggregator, const string& connAddr, const string& brokerId, const string& investorId, const string& passwd);
	~CQuoteProxy(void);

	CUstpFtdcMduserApi* MdApi(){ return m_pUserApi; }
	const string& InvestorId(){ return m_investorId; }
	const string& ConnectingIP() { return m_connectingIP; }

	bool Begin();
	void End();
	int WaitUntilEnd();

	void SubscribeMarketData(char** symbolArr, int symCount);
	void UnsubscribeMarketData(char** symbolArr, int symCount);
	void GetReady();
	bool IsReady(){ return m_isReady; }
	void OnQuoteReceived(CUstpFtdcDepthMarketDataField *pDepthMarketData);

private:
	string m_connAddr;
	string m_brokerId;
	string m_investorId;
	string m_passwd;
	boost::shared_ptr<CMdSpi> m_pMdSpi;
	string m_connectingIP;
	bool m_isReady;
	bool m_isRunning;
	boost::condition_variable m_readyEvent;
	boost::mutex m_mutex;
	boost::thread m_thQuoting;
	CUstpFtdcMduserApi* m_pUserApi;
	CQuoteAggregator* m_quoteAggregator;
};

typedef boost::shared_ptr<CQuoteProxy> QuoteProxyPtr;

