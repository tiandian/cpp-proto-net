#include "StdAfx.h"
#include "PriceBarGen.h"


CPriceBarGen::CPriceBarGen(void)
	: m_precision(0)
{
}


CPriceBarGen::~CPriceBarGen(void)
{
}

void CPriceBarGen::Init( int precision )
{
	m_precision = precision;
}

void CPriceBarGen::Calculate( entity::Quote* pQuote )
{

}
