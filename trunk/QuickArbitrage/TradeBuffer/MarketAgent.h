#pragma once

#include "ThostFtdcMdApi.h"
#include "MktDataResponse.h"
#include "QuoteListener.h"

#include <map>
#include <set>
#include <boost/uuid/uuid.hpp>
#include <vector>

class CMarketAgent
{
public:
	CMarketAgent(void);
	~CMarketAgent(void);

	bool Connect();
	bool Login(const char* brokerID, const char* userID, const char* password);
	void Logout( const char* brokerID, const char* userID );

	bool SubscribeQuotes(QuoteListener* pQuoteListener);
	bool ChangeQuotes(QuoteListener* pQuoteListener, std::vector<std::string>& changeSymbols );
	void UnsubscribeQuotes(boost::uuids::uuid& token);

private:
	void DispatchQuotes();

	int RequestIDIncrement();

	bool GetUpdateSymbolSet(std::vector<std::string>& subscribeArr, std::vector<std::string>& unsubscribeArr);
	bool SubmitToServer();

	bool SubmitSubscribes(std::vector<std::string>& subscribeArr);
	bool SubmitUnsubscribes(std::vector<std::string>& unsubscribeArr);

	CThostFtdcMdApi* m_pUserApi;
	CMktDataResponse m_mktResponse;

	typedef std::set<boost::uuids::uuid> TokenSet;
	typedef std::map<boost::uuids::uuid, QuoteListener*> QuoteListenerMap;
	typedef std::map<std::string, TokenSet> SymbolListenerMap;
	QuoteListenerMap m_mapQuoteListeners;
	SymbolListenerMap m_mapSymbolListeners;

	// «Î«Û±‡∫≈
	int m_iRequestID;
	std::set<std::string> m_subscribingSymbols;
};

