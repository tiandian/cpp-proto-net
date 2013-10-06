#include "StdAfx.h"
#include "TechDataRepo.h"
#include "FileOperations.h"
#include "TechStrategyDefs.h"

#include <boost/date_time.hpp>

CTechDataRepo::CTechDataRepo(void)
{
	// ensure the folder existing
	CreateFolderIfNotExists(HIST_DATA_FOLDER);
}


CTechDataRepo::~CTechDataRepo(void)
{
}

CPriceBarDataProxy* CTechDataRepo::Register( const string& symbol, int precision, bool histData, const boost::gregorian::date& tradingDay)
{
	boost::mutex::scoped_lock l(m_mutex);
	string dsKey;
	BuildKey(symbol, precision, &dsKey);
	PriceBarDSMapIter iterDS = m_priceBarDSMap.find(dsKey);
	if(iterDS != m_priceBarDSMap.end())
	{
		return iterDS->second->AddProxy();
	}
	else
	{
		// new PriceBarDataSource
		PriceBarDataSourcePtr ds(
			histData ? new CHistoryPriceBarDataSource(dsKey, tradingDay)
					 : new CPriceBarDataSource(dsKey, tradingDay));
		ds->Init(symbol, precision);
		m_priceBarDSMap.insert(make_pair(dsKey, ds));
		return ds->AddProxy();
	}
	return NULL;
}

bool CTechDataRepo::Unregister( CPriceBarDataProxy* proxy )
{
	boost::mutex::scoped_lock l(m_mutex);
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

void CTechDataRepo::BuildKey( const string& symbol, int precision, string* outKey )
{
	*outKey = boost::str(boost::format("%s-%d") % symbol % precision);
}
