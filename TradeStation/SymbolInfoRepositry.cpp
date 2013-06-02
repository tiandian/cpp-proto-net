#include "StdAfx.h"
#include "SymbolInfoRepositry.h"

const char EX_SHFE[] = "SHFE";
const char EX_DCE[] = "DCE";
const char EX_CZCE[] = "CZCE";
const char EX_FFEX[] = "FFEX";


CSymbolInfoRepositry::CSymbolInfoRepositry(void)
{
	m_prodExchMap.insert(make_pair("rb", EX_SHFE));
	m_prodExchMap.insert(make_pair("wr", EX_SHFE));
	m_prodExchMap.insert(make_pair("cu", EX_SHFE));
	m_prodExchMap.insert(make_pair("al", EX_SHFE));
	m_prodExchMap.insert(make_pair("ru", EX_SHFE));
	m_prodExchMap.insert(make_pair("fu", EX_SHFE));
	m_prodExchMap.insert(make_pair("zn", EX_SHFE));
	m_prodExchMap.insert(make_pair("au", EX_SHFE));
	m_prodExchMap.insert(make_pair("pb", EX_SHFE));
	m_prodExchMap.insert(make_pair("ag", EX_SHFE));

	m_prodExchMap.insert(make_pair("m", EX_DCE));
	m_prodExchMap.insert(make_pair("a", EX_DCE));
	m_prodExchMap.insert(make_pair("c", EX_DCE));
	m_prodExchMap.insert(make_pair("b", EX_DCE));
	m_prodExchMap.insert(make_pair("y", EX_DCE));
	m_prodExchMap.insert(make_pair("l", EX_DCE));
	m_prodExchMap.insert(make_pair("p", EX_DCE));
	m_prodExchMap.insert(make_pair("v", EX_DCE));
	m_prodExchMap.insert(make_pair("j", EX_DCE));

	m_prodExchMap.insert(make_pair("WT", EX_CZCE));
	m_prodExchMap.insert(make_pair("WS", EX_CZCE));
	m_prodExchMap.insert(make_pair("CF", EX_CZCE));
	m_prodExchMap.insert(make_pair("SR", EX_CZCE));
	m_prodExchMap.insert(make_pair("TA", EX_CZCE));
	m_prodExchMap.insert(make_pair("RO", EX_CZCE));
	m_prodExchMap.insert(make_pair("ER", EX_CZCE));
	m_prodExchMap.insert(make_pair("ME", EX_CZCE));
	m_prodExchMap.insert(make_pair("FG", EX_CZCE));
	m_prodExchMap.insert(make_pair("RM", EX_CZCE));
	m_prodExchMap.insert(make_pair("RS", EX_CZCE));

	m_prodExchMap.insert(make_pair("IF", EX_FFEX));
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

string CSymbolInfoRepositry::GetExchangeId( const string& instrument )
{
	string prodId;
	int prodLen = GetProductId(instrument, prodId);
	if(prodLen > 0)
	{
		map<string, string>::iterator iterExch = m_prodExchMap.find(prodId);
		if(iterExch != m_prodExchMap.end())
		{
			return iterExch->second;
		}
	}
	return "";
}
