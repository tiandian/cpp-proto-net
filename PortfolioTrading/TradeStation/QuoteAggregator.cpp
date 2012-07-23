#include "StdAfx.h"
#include "QuoteAggregator.h"
#include "globalmembers.h"

#include <boost/foreach.hpp>


CQuoteAggregator::CQuoteAggregator(void):
m_quoteAgent(NULL),
m_bufferRunner(boost::bind(&CQuoteAggregator::DispatchQuotes, this, _1))
{
}

CQuoteAggregator::~CQuoteAggregator(void)
{
	m_bufferRunner.Stop();
}

void CQuoteAggregator::Initialize( CQuoteAgent* quoteAgent )
{
	m_quoteAgent = quoteAgent;
}

void CQuoteAggregator::SubscribeQuotes( CQuoteListener* pQuoteListener )
{
	assert(pQuoteListener != NULL);

	WriteLock w_lock(m_lock);

	// know which symbols to subscribe
	vector<string>& regSymbols = pQuoteListener->GetSymbols();

	BOOST_FOREACH( const string& symb, regSymbols )
	{
		// add it to listener map
		m_symbolListeners.insert(make_pair(symb, pQuoteListener));
	}

	bool sumbit = SubmitToServer();
}

void CQuoteAggregator::ChangeQuotes( CQuoteListener* pQuoteListener, vector<string>& changeSymbols )
{
	WriteLock w_lock(m_lock);

}

void CQuoteAggregator::UnsubscribeQuotes(CQuoteListener* pQuoteListener)
{
	WriteLock w_lock(m_lock);

	// know which symbols to Unsubscribe
	vector<string>& unregSymbols = pQuoteListener->GetSymbols();
	
	BOOST_FOREACH( const string& symb, unregSymbols )
	{
		pair<SymbolListenerMapIter, SymbolListenerMapIter> ret = m_symbolListeners.equal_range(symb);
		for (SymbolListenerMapIter it = ret.first; it != ret.second; ++it)
		{
			if(it->second == pQuoteListener)
			{
				// find it;
				m_symbolListeners.erase(it);
				break;
			}
		}
	}

	bool sumbit = SubmitToServer();

	pQuoteListener->GetSymbols().clear();
}

void CQuoteAggregator::OnSubscribeCompleted()
{

}

void CQuoteAggregator::OnUnsubscribeCompleted()
{

}

void CQuoteAggregator::OnQuoteReceived( entity::Quote* pQuote )
{
	// wrap row pointer in shared pointer and manage the memory
	m_bufferRunner.Enqueue(boost::shared_ptr<entity::Quote>(pQuote));
}

void CQuoteAggregator::DispatchQuotes( boost::shared_ptr<entity::Quote>& pQuote )
{
	ReadLock lock(m_lock);
	const string& symb = pQuote->symbol();
	pair<SymbolListenerMapIter, SymbolListenerMapIter> ret = m_symbolListeners.equal_range(symb);
	for (SymbolListenerMapIter it = ret.first; it != ret.second; ++it)
	{
		(it->second)->OnQuoteRecevied(pQuote);
	}
}

bool CQuoteAggregator::GetUpdateSymbolSet( vector<string>& subscribeArr, vector<string>& unsubscribeArr )
{
	bool needSubmit = true;

	// collect symbols to be subscribed
	set<string> currentSymbols;
	for (SymbolListenerMap::iterator iter = m_symbolListeners.begin(); iter != m_symbolListeners.end(); ++iter)
	{
		currentSymbols.insert(iter->first);
	}

	// the largest possible size of subscribe array will be currentSymbols' size
	subscribeArr.resize(currentSymbols.size());
	vector<string>::iterator subscribeDiffIter = set_difference(currentSymbols.begin(), currentSymbols.end(),
		m_subscribingSymbols.begin(), m_subscribingSymbols.end(),
		subscribeArr.begin());
	int subDiffCount = subscribeDiffIter - subscribeArr.begin();
	subscribeArr.resize(subDiffCount);

	// the same thing with unsubscribeArr and m_subscribingSymbols which is last time of subscribing symbols
	unsubscribeArr.resize(m_subscribingSymbols.size());
	vector<string>::iterator unsubDiffIter = set_difference(m_subscribingSymbols.begin(), m_subscribingSymbols.end(),
		currentSymbols.begin(), currentSymbols.end(),
		unsubscribeArr.begin());
	int unSubDiffCount = unsubDiffIter - unsubscribeArr.begin();
	unsubscribeArr.resize(unSubDiffCount);

	if(subscribeArr.size() == 0 && unsubscribeArr.size() == 0)
	{
		// no difference at all, don't need to submit to server
		needSubmit = false;
	}
	else
	{
		// update subscribing symbols
		m_subscribingSymbols.swap(currentSymbols);
	}

	return needSubmit;
}

bool CQuoteAggregator::SubmitToServer()
{
	bool retVal = false;

	// update m_subscribingSymbols to latest and get subscribe array and unsubscribe array
	vector<string> subscribeArr, unsubscribeArr;
	bool needSubmit = GetUpdateSymbolSet(subscribeArr, unsubscribeArr);
	if(needSubmit)
	{
		bool succ = m_quoteAgent->SubscribesQuotes(subscribeArr);

		succ = m_quoteAgent->UnSubscribesQuotes(unsubscribeArr);
	}
	else
	{
		logger.Trace("Subscribed symbols didn't change, no need to submit.");
		retVal = true;
	}

	return retVal;
}
