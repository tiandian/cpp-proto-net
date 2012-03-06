#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <boost/uuid/uuid.hpp>

class QuoteListener
{
protected:
	QuoteListener(void);
	~QuoteListener(void);

public:
	boost::uuids::uuid& GetUuid(){ return m_token; }
	void SetUuid(boost::uuids::uuid& token){ m_token = token; }

	std::vector<std::string>& GetSymbols(){ return m_symbols; }
	void SetSymbols(std::vector<std::string>& reg_symbols);
	
	virtual void OnQuoteRecevied() = 0;

protected:
	boost::uuids::uuid m_token;
	std::vector<std::string> m_symbols;

};

