#include "StdAfx.h"
#include "PortfolioManager.h"
#include "globalmembers.h"

#include <boost/format.hpp>


CPortfolioManager::CPortfolioManager(void):
m_quoteAggregator(NULL)
{
}


CPortfolioManager::~CPortfolioManager(void)
{
}

CPortfolio* CPortfolioManager::Get( const string& portfId )
{
	PortfolioMapIter portIter = m_mapPortfolios.find(portfId);
	if(portIter != m_mapPortfolios.end())
		return (portIter->second).get();

	return NULL;
}

void CPortfolioManager::Add( CPortfolio* portfolio )
{
	const string& pid = portfolio->ID();
	PortfolioPtr portfWrapper(portfolio);
	m_mapPortfolios.insert(make_pair(pid, portfWrapper));
	portfolio->SetManager(this);

	logger.Info(boost::str(boost::format("Portfolio(%s) added") % pid.c_str()));
}

void CPortfolioManager::Remove( const string& portfId )
{
	m_mapPortfolios.erase(portfId);
}

void CPortfolioManager::PortfolioPositionChange( const MultiLegOrderPtr& multiLegOrder )
{
	CPortfolio* pPortf = Get(multiLegOrder->portfolioid());
	if(pPortf != NULL)
	{
		if(multiLegOrder->orderid() == multiLegOrder->openorderid())
		{
			// the order is to open position
			pPortf->AddPosition(multiLegOrder);
		}
		else
		{
			pPortf->RemovePosition(multiLegOrder);
		}
	}
}

void CPortfolioManager::ChangePortfolioResubmitter( const string& portfId, COrderResubmitter* pResubmitter, bool isAdding )
{
	CPortfolio* pPortf = Get(portfId);
	if(pPortf != NULL)
	{
		if(isAdding)
		{
			pPortf->AddOrderResubmitter(pResubmitter);
		}
		else
			pPortf->RemoveOrderResubmitter(pResubmitter);
	}
}

void CPortfolioManager::PortfolioPlaceOrderDone( const string& portfId )
{
	CPortfolio* pPortf = Get(portfId);
	if(pPortf != NULL)
	{
		pPortf->EndPlaceOrder();
	}
}
