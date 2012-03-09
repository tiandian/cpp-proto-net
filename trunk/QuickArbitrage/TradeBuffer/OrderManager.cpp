#include "StdAfx.h"
#include "OrderManager.h"
#include "QuoteAggregator.h"

extern CQuoteAggregator g_quoteAggregator;

COrderManager::COrderManager(void)
{
}


COrderManager::~COrderManager(void)
{
}

void COrderManager::OnQuoteRecevied(boost::shared_ptr<CTP::Quote>& pQuote)
{

}

void COrderManager::Initialize()
{
	
}
