#include "StdAfx.h"
#include "QuoteListener.h"

#include <boost/uuid/uuid_generators.hpp>

CQuoteListener::CQuoteListener(void)
{
}


CQuoteListener::~CQuoteListener(void)
{
}

void CQuoteListener::SetSymbols( std::vector<std::string>& reg_symbols )
{
	m_symbols.resize(reg_symbols.size());
	std::copy(reg_symbols.begin(), reg_symbols.end(), m_symbols.begin());
}