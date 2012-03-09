#include "StdAfx.h"
#include "ConsoleClient.h"
#include "LogManager.h"

extern CLogManager logger;

CConsoleClient::CConsoleClient(void):
	m_streamOut(std::stringstream::out)
{
}


CConsoleClient::~CConsoleClient(void)
{
}

void CConsoleClient::ProcessQuote( boost::shared_ptr<CTP::Quote>& pQuote )
{
	m_streamOut.str("");
	m_streamOut << pQuote->symbol() << ", ";
	m_streamOut << pQuote->last() << ", ";
	m_streamOut << pQuote->update_time() << endl;

	logger.Info(m_streamOut.str());
}
