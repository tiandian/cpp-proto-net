#pragma once

#include "SymbolInfo.h"

#include <map>
#include <string>
#include <boost/shared_ptr.hpp>

using namespace std;

typedef boost::shared_ptr<CSymbolInfo> SymbolInfoPtr;

class CSymbolInfoRepositry
{
public:
	CSymbolInfoRepositry(void);
	~CSymbolInfoRepositry(void);

	CSymbolInfo* Get(const char* instrument);
	string GetExchangeId(const string& instrument);

private:
	int GetProductId(const string& symbol, string& prodId);
	
	typedef map<string, SymbolInfoPtr> SymbolInfoMap;
	typedef SymbolInfoMap::iterator SymbolInfoMapIter;

	SymbolInfoMap m_symbolInfoMap;

	map<string, string> m_prodExchMap;
};

