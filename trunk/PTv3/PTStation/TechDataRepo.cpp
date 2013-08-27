#include "StdAfx.h"
#include "TechDataRepo.h"


CTechDataRepo::CTechDataRepo(void)
{
}


CTechDataRepo::~CTechDataRepo(void)
{
}

CPriceBarDataProxy* CTechDataRepo::Register( const string& symbol, int precision, TA_INDICATOR indicator, map<string, double>* indicatorParams )
{
	string dsKey;
	BuildKey(symbol, precision, indicator, &dsKey);
	PriceBarDSMapIter iterDS = m_priceBarDSMap.find(dsKey);
	if(iterDS != m_priceBarDSMap.end())
	{
		return iterDS->second->AddProxy();
	}
	else
	{
		// new PriceBarDataSource
		PriceBarDataSourcePtr ds(new CPriceBarDataSource(dsKey));
		ds->Init(precision, indicator);
		m_priceBarDSMap.insert(make_pair(dsKey, ds));
		return ds->AddProxy();
	}
	return NULL;
}

bool CTechDataRepo::Unregister( CPriceBarDataProxy* proxy )
{
	string dsKey = proxy->DataSourceId();
	PriceBarDSMapIter iterDS = m_priceBarDSMap.find(dsKey);
	if(iterDS != m_priceBarDSMap.end())
	{
		iterDS->second->RemoveProxy(proxy);
		if(iterDS->second->IsDisposable())
		{
			m_priceBarDSMap.erase(iterDS);
			return true;
		}
	}
	return false;
}

void CTechDataRepo::BuildKey( const string& symbol, int precision, TA_INDICATOR indicator, string* outKey )
{
	*outKey = boost::str(boost::format("%s-%d-%d") % symbol % precision % indicator);
}
