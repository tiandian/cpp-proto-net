#pragma once

#include "QuoteListener.h"
#include "MarketAgent.h"
#include "MarketAgentCallback.h"
#include "BufferRunner.h"

#include <map>
#include <set>
#include <vector>
#include <boost/smart_ptr.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/thread/locks.hpp> 


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
	virtual void OnQuoteReceived(CTP::Quote* pQuote);

private:
	void DispatchQuotes(boost::shared_ptr<CTP::Quote>& pQuote);

	bool GetUpdateSymbolSet(std::vector<std::string>& subscribeArr, std::vector<std::string>& unsubscribeArr);
	bool SubmitToServer();

	CMarketAgent* m_pMarketAgent;

	typedef std::set<boost::uuids::uuid> TokenSet;
	typedef std::map<boost::uuids::uuid, QuoteListener*> QuoteListenerMap;
	typedef std::map<std::string, TokenSet> SymbolListenerMap;
	QuoteListenerMap m_mapQuoteListeners;
	SymbolListenerMap m_mapSymbolListeners;

	std::set<std::string> m_subscribingSymbols;

	CBufferRunner< boost::shared_ptr<CTP::Quote> >* m_pBufferRunner;

	typedef boost::shared_mutex Lock; 
	typedef boost::unique_lock< boost::shared_mutex > WriteLock; 
	typedef boost::shared_lock< boost::shared_mutex >  ReadLock; 

	Lock m_lock;
};

