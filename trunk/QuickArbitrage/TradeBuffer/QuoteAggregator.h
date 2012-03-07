#pragma once

#include "QuoteListener.h"
#include "MarketAgent.h"
#include "MarketAgentCallback.h"

#include <map>
#include <set>
#include <boost/uuid/uuid.hpp>
#include <vector>

class CQuoteAggregator : public CMarketAgentCallback
{
public:
	CQuoteAggregator(void);
	~CQuoteAggregator(void);

	void Initialize(CMarketAgent* pAgent);

	bool SubscribeQuotes(QuoteListener* pQuoteListener);
	bool ChangeQuotes(QuoteListener* pQuoteListener, std::vector<std::string>& changeSymbols );
	void UnsubscribeQuotes(boost::uuids::uuid& token);

	virtual void OnSubscribeCompleted();
	virtual void OnUnsubscribeCompleted();
	virtual void OnQuoteReceived();

private:
	void DispatchQuotes();

	bool GetUpdateSymbolSet(std::vector<std::string>& subscribeArr, std::vector<std::string>& unsubscribeArr);
	bool SubmitToServer();

	CMarketAgent* m_pMarketAgent;

	typedef std::set<boost::uuids::uuid> TokenSet;
	typedef std::map<boost::uuids::uuid, QuoteListener*> QuoteListenerMap;
	typedef std::map<std::string, TokenSet> SymbolListenerMap;
	QuoteListenerMap m_mapQuoteListeners;
	SymbolListenerMap m_mapSymbolListeners;

	std::set<std::string> m_subscribingSymbols;
};

