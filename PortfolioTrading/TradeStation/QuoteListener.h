#pragma once

#include "../Entity/gen/cpp/quote.pb.h"

#include <string>
#include <vector>
#include <algorithm>
#include <boost/smart_ptr.hpp>
#include <boost/uuid/uuid.hpp>

class CQuoteListener
{
protected:
	CQuoteListener(void);
	virtual ~CQuoteListener(void);

public:
	boost::uuids::uuid& GetUuid(){ return m_token; }
	void SetUuid(boost::uuids::uuid& token){ m_token = token; }

	std::vector<std::string>& GetSymbols(){ return m_symbols; }
	void SetSymbols(std::vector<std::string>& reg_symbols);

	virtual void OnQuoteRecevied(boost::shared_ptr<entity::Quote>& pQuote) = 0;

protected:
	boost::uuids::uuid m_token;
	std::vector<std::string> m_symbols;
};

