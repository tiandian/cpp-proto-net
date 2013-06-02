#pragma once

#include "../Entity/gen/cpp/quote.pb.h"

#include <string>
#include <vector>
#include <algorithm>
#include <boost/smart_ptr.hpp>

class CQuoteListener
{
protected:
	CQuoteListener(void);
	virtual ~CQuoteListener(void);

public:
	bool Registered(){ return m_symbols.size() > 0; }
	
	std::vector<std::string>& GetSymbols(){ return m_symbols; }
	void SetSymbols(std::vector<std::string>& reg_symbols);

	virtual void OnQuoteRecevied(boost::shared_ptr<entity::Quote>& pQuote) = 0;

protected:
	std::vector<std::string> m_symbols;
};

