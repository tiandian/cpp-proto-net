#pragma once

#include "Portfolio.h"
#include "QuoteAggregator.h"

#include <string>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

using namespace std;

typedef boost::shared_ptr<CPortfolio> PortfolioPtr;
typedef boost::function<void(entity::PortfolioItem*)> PushPorfolioFunc;

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

private:

	typedef map<string, PortfolioPtr> PortfolioMap;
	typedef PortfolioMap::iterator PortfolioMapIter;

	PortfolioMap m_mapPortfolios;

	CQuoteAggregator* m_quoteAggregator;

	PushPorfolioFunc m_pushPortfolioFunc;
};

