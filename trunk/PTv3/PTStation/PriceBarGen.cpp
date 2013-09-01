#include "StdAfx.h"
#include "PriceBarGen.h"
#include "ExchangeRelatedCalculation.h"
#include <boost/chrono.hpp>

CPriceBarGen::CPriceBarGen(void)
	: m_precision(0)
{
}


CPriceBarGen::~CPriceBarGen(void)
{
}

void CPriceBarGen::Init( const string& symbol, int precision )
{
	m_symbol = symbol;
	m_precision = precision;
}

void CPriceBarGen::Calculate( entity::Quote* pQuote )
{

}
