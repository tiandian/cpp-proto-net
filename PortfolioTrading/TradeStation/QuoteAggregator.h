#pragma once

#include "QuoteListener.h"
#include "QuoteAgent.h"
#include "QuoteAgentCallback.h"
#include "BufferRunner.h"

#include <string>
#include <vector>
#include <set>
#include <map>
#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>

using namespace std;

class CQuoteAggregator : public CQuoteAgentCallback
{
public:
	CQuoteAggregator(void);
	~CQuoteAggregator(void);

	void Initialize(CQuoteAgent* quoteAgent);

	void SubscribeQuotes(CQuoteListener* pQuoteListener);
	void ChangeQuotes(CQuoteListener* pQuoteListener, vector<string>& changeSymbols );
	void UnsubscribeQuotes(CQuoteListener* pQuoteListener);

	virtual void OnSubscribeCompleted();
	virtual void OnUnsubscribeCompleted();
	virtual void OnQuoteReceived(entity::Quote* pQuote);

	void DelaySubmit() { m_bDelaySubmit = true; }
	void SubmitSubscription();

private:

	void DispatchQuotes(boost::shared_ptr<entity::Quote>& pQuote);

	bool GetUpdateSymbolSet(vector<string>& subscribeArr, vector<string>& unsubscribeArr);
	bool SubmitToServer();

	typedef multimap<string, CQuoteListener*> SymbolListenerMap;
	typedef SymbolListenerMap::iterator SymbolListenerMapIter;

	typedef boost::shared_mutex Lock; 
	typedef boost::unique_lock< boost::shared_mutex > WriteLock; 
	typedef boost::shared_lock< boost::shared_mutex >  ReadLock; 

	Lock m_lock;

	SymbolListenerMap m_symbolListeners;
	std::set<std::string> m_subscribingSymbols;
	CQuoteAgent* m_quoteAgent;

	CBufferRunner< boost::shared_ptr<entity::Quote> > m_bufferRunner;

	bool m_bDelaySubmit;
};

