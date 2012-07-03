#include "StdAfx.h"
#include "QuoteAgent.h"


CQuoteAgent::CQuoteAgent(void)
{
}


CQuoteAgent::~CQuoteAgent(void)
{
}

bool CQuoteAgent::Open( const string& address )
{
	return false;
}

void CQuoteAgent::Close()
{

}

bool CQuoteAgent::Login( const string& brokerId, const string& userId, const string& password )
{
	return false;
}

void CQuoteAgent::Logout()
{

}

bool CQuoteAgent::SubscribesQuotes( vector<string>& subscribeArr )
{
	return false;
}

bool CQuoteAgent::UnSubscribesQuotes( vector<string>& unSubscribeArr )
{
	return false;
}
