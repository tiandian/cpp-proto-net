#include "StdAfx.h"
#include "OrderProcessor.h"
#include "MarketAgent.h"
#include "ClientAgent.h"

#include <vector>

using namespace std;

extern CMarketAgent g_marketAgent;
extern CClientAgent g_clientAgent;

COrderProcessor::COrderProcessor(void):
	m_bufferRunner(boost::bind(&COrderProcessor::ProcessQuote, this, _1))
{
}


COrderProcessor::~COrderProcessor(void)
{
}

void COrderProcessor::OnSubscribeCompleted()
{

}

void COrderProcessor::OnUnsubscribeCompleted()
{

}

void COrderProcessor::OnQuoteReceived( CQuote* pQuote )
{
	boost::shared_ptr<CQuote> wrappedQuote(pQuote);
	m_bufferRunner.Enqueue(wrappedQuote);
}

void COrderProcessor::ForwardQuote( boost::shared_ptr<CQuote>& pQuote )
{
	g_clientAgent.UpdateQuote(pQuote);
}

void COrderProcessor::SetSymbol( const std::string& symb )
{
	m_currentSymbol = symb;
	vector<string> regSymbols;
	regSymbols.push_back(m_currentSymbol);
	bool succ = g_marketAgent.SubscribesQuotes(regSymbols);
}

void COrderProcessor::ProcessQuote( boost::shared_ptr<CQuote>& pQuote )
{
	// TODO: test condition and fire trigger

	ForwardQuote(pQuote);
}
