#include "StdAfx.h"
#include "OrderProcessor.h"
#include "MarketAgent.h"
#include "ClientAgent.h"
#include "LogManager.h"



using namespace std;

extern CMarketAgent g_marketAgent;
extern CClientAgent g_clientAgent;
extern CLogManager	logger;

COrderProcessor::COrderProcessor(void):
	m_bufferRunner(boost::bind(&COrderProcessor::ProcessQuote, this, _1)),
	m_currentSymbols(1)
{
}


COrderProcessor::~COrderProcessor(void)
{
	m_bufferRunner.Stop();
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
	boost::shared_ptr<CMessage> msgPack = pQuote;
	g_clientAgent.Publish(msgPack);
}


void COrderProcessor::SetSymbol( const std::string& symb )
{
	if(!m_currentSymbols[0].empty())
	{
		g_marketAgent.UnSubscribesQuotes(m_currentSymbols);
	}
	m_currentSymbols[0] = symb;
	if(!symb.empty())
	{
		bool subSucc = g_marketAgent.SubscribesQuotes(m_currentSymbols);
	}
}

void COrderProcessor::ProcessQuote( boost::shared_ptr<CQuote>& pQuote )
{
	// TODO: test condition and fire trigger

	ForwardQuote(pQuote);
}

void COrderProcessor::Initialize()
{
	g_marketAgent.SetCallbackHanlder(this);

	m_bufferRunner.Start();

	logger.Info("Quote aggregator initialized.");
}
