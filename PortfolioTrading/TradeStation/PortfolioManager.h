#pragma once

#include "Portfolio.h"
#include "QuoteAggregator.h"
#include "multilegorderptr.h"

#include <string>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

using namespace std;

typedef boost::shared_ptr<CPortfolio> PortfolioPtr;
typedef boost::function<void(entity::PortfolioItem*)> PushPorfolioFunc;

typedef boost::function<void(CPortfolio*, int)> NotifiyPortfolioOpenPosiFunc;

class CPortfolioManager
{
public:
	CPortfolioManager(void);
	~CPortfolioManager(void);

	void SetQuoteAggregator(CQuoteAggregator* quoteAggregator)
	{
		m_quoteAggregator = quoteAggregator;
	}

	CQuoteAggregator* QuoteAggregator(){ return m_quoteAggregator; }

	void Add(CPortfolio* portfolio);
	CPortfolio* Get(const string& portfId);
	void Remove(const string& portfId);

	void PortfolioPositionChange(const MultiLegOrderPtr& multiLegOrder);

	void PublishPortfolioUpdate(entity::PortfolioItem* pPortfolio)
	{
		if(!m_pushPortfolioFunc.empty())
		{
			m_pushPortfolioFunc(pPortfolio);
		}
	}
	void SetPushPortfolioFunc(PushPorfolioFunc funcPushPortf)
	{
		m_pushPortfolioFunc = funcPushPortf;
	}

	void NotifyOpenPosition(CPortfolio* portf, int qty)
	{
		if(!m_portfOpenPosiFunc.empty())
		{
			m_portfOpenPosiFunc(portf, qty);
		}
	}

	void SetOpenPortfolioFunc(NotifiyPortfolioOpenPosiFunc funcOpenPosi)
	{
		m_portfOpenPosiFunc = funcOpenPosi;
	}

private:

	typedef map<string, PortfolioPtr> PortfolioMap;
	typedef PortfolioMap::iterator PortfolioMapIter;

	PortfolioMap m_mapPortfolios;

	CQuoteAggregator* m_quoteAggregator;

	PushPorfolioFunc m_pushPortfolioFunc;
	NotifiyPortfolioOpenPosiFunc m_portfOpenPosiFunc;
};

