#include "StdAfx.h"
#include "SymbolInfoRepositry.h"


CSymbolInfoRepositry::CSymbolInfoRepositry(void)
{
	m_prodExchMap.insert(make_pair("rb", "SHFE"));
	m_prodExchMap.insert(make_pair("wr", "SHFE"));
	m_prodExchMap.insert(make_pair("cu", "SHFE"));
	m_prodExchMap.insert(make_pair("al", "SHFE"));
	m_prodExchMap.insert(make_pair("ru", "SHFE"));
	m_prodExchMap.insert(make_pair("fu", "SHFE"));
	m_prodExchMap.insert(make_pair("zn", "SHFE"));
	m_prodExchMap.insert(make_pair("au", "SHFE"));
	m_prodExchMap.insert(make_pair("pb", "SHFE"));
	m_prodExchMap.insert(make_pair("ag", "SHFE"));

	m_prodExchMap.insert(make_pair("m", "DCE"));
	m_prodExchMap.insert(make_pair("a", "DCE"));
	m_prodExchMap.insert(make_pair("c", "DCE"));
	m_prodExchMap.insert(make_pair("b", "DCE"));
	m_prodExchMap.insert(make_pair("y", "DCE"));
	m_prodExchMap.insert(make_pair("l", "DCE"));
	m_prodExchMap.insert(make_pair("p", "DCE"));
	m_prodExchMap.insert(make_pair("v", "DCE"));
	m_prodExchMap.insert(make_pair("j", "DCE"));

	m_prodExchMap.insert(make_pair("wt", "CZCE"));
	m_prodExchMap.insert(make_pair("ws", "CZCE"));
	m_prodExchMap.insert(make_pair("cf", "CZCE"));
	m_prodExchMap.insert(make_pair("sr", "CZCE"));
	m_prodExchMap.insert(make_pair("ta", "CZCE"));
	m_prodExchMap.insert(make_pair("ro", "CZCE"));
	m_prodExchMap.insert(make_pair("er", "CZCE"));
	m_prodExchMap.insert(make_pair("me", "CZCE"));
	m_prodExchMap.insert(make_pair("fg", "CZCE"));
	m_prodExchMap.insert(make_pair("rm", "CZCE"));
	m_prodExchMap.insert(make_pair("rs", "CZCE"));

	m_prodExchMap.insert(make_pair("if", "FFEX"));
}

CSymbolInfoRepositry::~CSymbolInfoRepositry(void)
{
}

CSymbolInfo* CSymbolInfoRepositry::Get( const char* instrument )
{
	string symbol = instrument;
	SymbolInfoMapIter iterSI = m_symbolInfoMap.find(symbol);
	if(iterSI != m_symbolInfoMap.end())
	{
		return (iterSI->second).get();
	}
	else
	{
		string prodId;
		int prodLen = GetProductId(symbol, prodId);
		map<string, string>::iterator iterExch = m_prodExchMap.find(prodId);
		if(iterExch != m_prodExchMap.end())
		{
			SymbolInfoPtr siPtr(new CSymbolInfo(symbol, iterExch->second, prodId));
			m_symbolInfoMap.insert(make_pair(siPtr->Instrument(), siPtr));
			return siPtr.get();
		}
	}

	return NULL;
}

int CSymbolInfoRepositry::GetProductId( const string& symbol, string& prodId )
{
	int nonNumberPos = 0;
	while(symbol[nonNumberPos] > '9')
		++nonNumberPos;

	prodId = symbol.substr(0, nonNumberPos);
	return nonNumberPos;
}
