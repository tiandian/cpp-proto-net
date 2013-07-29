#include "StdAfx.h"
#include "SymbolInfo.h"


CSymbolInfo::CSymbolInfo(const string& symbol, const string& exchId, const string& prodId):
m_ready(false)
{
	Instrument(symbol);
	ExchangeID(exchId);
	ProductID(prodId);
	ExchangeInstID(symbol);
	VolumeMultiple(0);
	PriceTick(0);
}

CSymbolInfo::~CSymbolInfo(void)
{
}
