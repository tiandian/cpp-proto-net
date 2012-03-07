#include "StdAfx.h"
#include "QuoteAggregator.h"
#include "LogManager.h"
#include "Configuration.h"

#include <sstream>
#include <algorithm>
#include <boost/smart_ptr.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#define SYMBOL_MAX_LENGTH 10

using namespace std;

extern CConfiguration config;
extern CLogManager	logger;

CQuoteAggregator::CQuoteAggregator(void):
	m_pMarketAgent(NULL)
{
}


CQuoteAggregator::~CQuoteAggregator(void)
{
}

bool CQuoteAggregator::SubscribeQuotes( QuoteListener* pQuoteListener )
{
	assert(pQuoteListener != NULL);
	assert(pQuoteListener->GetUuid().is_nil());

	// generate token for quote listener
	boost::uuids::random_generator gen;
	boost::uuids::uuid token = gen();

	pQuoteListener->SetUuid(token);

	// add it to listener map
	m_mapQuoteListeners.insert( pair<boost::uuids::uuid, QuoteListener*>(token, pQuoteListener));

	// know which symbols to subscribe
	vector<string>& regSymbols = pQuoteListener->GetSymbols();
	for(vector<string>::iterator iter = regSymbols.begin(); iter != regSymbols.end(); ++iter)
	{
		string& s = *iter;
		// see if the symbol already registered
		SymbolListenerMap::iterator mapIter = m_mapSymbolListeners.find(s);
		if(mapIter == m_mapSymbolListeners.end())
		{
			// not registered yet, need to create a new vector for token;
			TokenSet tokenSet;
			tokenSet.insert(token);
			m_mapSymbolListeners.insert(pair<std::string, TokenSet>(s, tokenSet));
		}
		else
		{
			// already registered, just add the token to existing token vector
			(mapIter->second).insert(token);
		}
	}

	stringstream info(stringstream::out);
	info << "A new quote listener subscribing with uuid(' " << token << ")'";
	logger.Trace(info.str());

	bool sumbit = SubmitToServer();

	return true;
}

bool CQuoteAggregator::ChangeQuotes( QuoteListener* pQuoteListener, std::vector<std::string>& changeSymbols )
{
	assert(pQuoteListener != NULL);

	boost::uuids::uuid& token = pQuoteListener->GetUuid();

	QuoteListenerMap::iterator foundIter = m_mapQuoteListeners.find(token);
	if(foundIter != m_mapQuoteListeners.end())
	{
		// found this token
		vector<string>& origSymbols = (foundIter->second)->GetSymbols();

		// Find out newly added symbols in newSymbols but not in origSymbols
		vector<string> added(changeSymbols.size());
		vector<string>::iterator addedIter = set_difference(changeSymbols.begin(), changeSymbols.end(),
			origSymbols.begin(), origSymbols.end(),
			added.begin());
		added.shrink_to_fit();

		for (vector<string>::iterator adIt = added.begin(); adIt != added.end(); ++adIt)
		{
			string& s = *adIt;
			// see if the symbol already registered
			SymbolListenerMap::iterator mapIter = m_mapSymbolListeners.find(s);
			if(mapIter == m_mapSymbolListeners.end())
			{
				// not registered yet, need to create a new vector for token;
				TokenSet tokenSet;
				tokenSet.insert(token);
				m_mapSymbolListeners.insert(pair<std::string, TokenSet>(s, tokenSet));
			}
			else
			{
				// already registered, just add the token to existing token vector
				(mapIter->second).insert(token);
			}
		}

		// Find out those in origSymbols but not in newSymbols, remove them
		vector<string> toRemoved(origSymbols.size());
		vector<string>::iterator removedIter = set_difference(origSymbols.begin(), origSymbols.end(),
			changeSymbols.begin(), changeSymbols.end(),
			toRemoved.begin());
		toRemoved.shrink_to_fit();

		for (vector<string>::iterator delIt = toRemoved.begin(); delIt != toRemoved.end(); ++delIt)
		{
			string& s = *delIt;
			SymbolListenerMap::iterator foundSymbol = m_mapSymbolListeners.find(s);
			if(foundSymbol != m_mapSymbolListeners.end())
			{
				TokenSet& tokens = foundSymbol->second;
				tokens.erase(token);

				if(tokens.size() == 0)
				{
					// if there is no token for the symbol, erase this from map
					m_mapSymbolListeners.erase(foundSymbol);
				}
			}
		}

		// The last, update to changed symbols set
		(foundIter->second)->SetSymbols(changeSymbols);

		// Finally, update to server
		bool sumbit = SubmitToServer();
	}

	return true;
}

void CQuoteAggregator::UnsubscribeQuotes( boost::uuids::uuid& token )
{
	QuoteListenerMap::iterator foundIter = m_mapQuoteListeners.find(token);
	if(foundIter != m_mapQuoteListeners.end())
	{
		// found this token
		vector<string>& symbols = (foundIter->second)->GetSymbols();
		boost::uuids::uuid token = (foundIter->second)->GetUuid();

		for(vector<string>::iterator it = symbols.begin(); it != symbols.end(); ++it)
		{
			SymbolListenerMap::iterator foundSymbol = m_mapSymbolListeners.find(*it);
			if(foundSymbol != m_mapSymbolListeners.end())
			{
				TokenSet& tokens = foundSymbol->second;
				tokens.erase(token);

				if(tokens.size() == 0)
				{
					// if there is no token for the symbol, erase this from map
					m_mapSymbolListeners.erase(foundSymbol);
				}
			}
		}
		// erase this quote listener
		m_mapQuoteListeners.erase(foundIter);

		bool sumbit = SubmitToServer();
	}
}

bool CQuoteAggregator::GetUpdateSymbolSet( std::vector<std::string>& subscribeArr, std::vector<std::string>& unsubscribeArr )
{
	bool needSubmit = true;

	// collect symbols to be subscribed
	set<string> currentSymbols;
	for (SymbolListenerMap::iterator iter = m_mapSymbolListeners.begin(); iter != m_mapSymbolListeners.end(); ++iter)
	{
		currentSymbols.insert(iter->first);
	}

	// the largest possible size of subscribe array will be currentSymbols' size
	subscribeArr.resize(currentSymbols.size());
	vector<string>::iterator subscribeDiff = set_difference(currentSymbols.begin(), currentSymbols.end(),
		m_subscribingSymbols.begin(), m_subscribingSymbols.end(),
		subscribeArr.begin());
	subscribeArr.shrink_to_fit();

	// the same thing with unsubscribeArr and m_subscribingSymbols which is last time of subscribing symbols
	unsubscribeArr.resize(m_subscribingSymbols.size());
	vector<string>::iterator unsubDiff = set_difference(m_subscribingSymbols.begin(), m_subscribingSymbols.end(),
		currentSymbols.begin(), currentSymbols.end(),
		unsubscribeArr.begin());
	unsubscribeArr.shrink_to_fit();

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
		bool succ = m_pMarketAgent->SubscribesQuotes(subscribeArr);

		succ = m_pMarketAgent->UnSubscribesQuotes(unsubscribeArr);
	}
	else
	{
		logger.Trace("Subscribed symbols didn't change, no need to submit.");
		retVal = true;
	}

	return retVal;
}

void CQuoteAggregator::DispatchQuotes()
{

}

void CQuoteAggregator::Initialize( CMarketAgent* pAgent )
{
	m_pMarketAgent = pAgent;
	
	// Register callback
	m_pMarketAgent->SetCallbackHanlder(this);
}

void CQuoteAggregator::OnSubscribeCompleted()
{

}

void CQuoteAggregator::OnUnsubscribeCompleted()
{

}

void CQuoteAggregator::OnQuoteReceived()
{

}
