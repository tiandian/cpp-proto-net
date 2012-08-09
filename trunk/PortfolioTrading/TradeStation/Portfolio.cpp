#include "StdAfx.h"
#include "Portfolio.h"
#include "PortfolioManager.h"

#include <boost/foreach.hpp>

CPortfolio::CPortfolio(void):
m_porfMgr(NULL)
{
}


CPortfolio::~CPortfolio(void)
{
}

CLeg* CPortfolio::AddLeg(entity::LegItem* pLegItem)
{
	int legCount = m_vecLegs.size();
	LegPtr leg(new CLeg(legCount + 1));
	leg->SetItem(pLegItem);
	m_vecLegs.push_back(leg);
	return leg.get();
}

int CPortfolio::Count()
{
	return m_vecLegs.size();
}

CLeg* CPortfolio::GetLeg( int legId )
{
	if(legId <= m_vecLegs.size())
		return m_vecLegs[legId - 1].get();
}

void CPortfolio::SetItem( entity::PortfolioItem* pPortfItem )
{
	m_innerItem = PortfItemPtr(pPortfItem);
	google::protobuf::RepeatedPtrField< ::entity::LegItem > legs = pPortfItem->legs();
	for (google::protobuf::RepeatedPtrField< ::entity::LegItem >::iterator legIter = legs.begin();
		legIter != legs.end(); ++legIter)
	{
		AddLeg(&(*legIter));
	}
}

void CPortfolio::OnQuoteRecevied( boost::shared_ptr<entity::Quote>& pQuote )
{
	double diff = 0;
	for(vector<LegPtr>::iterator iter = m_vecLegs.begin(); iter != m_vecLegs.end(); ++iter)
	{
		entity::LegItem* leg = iter->get()->Item();
		
		if(leg->symbol() == pQuote->symbol())
		{
			leg->set_last(pQuote->last());
		}

		double legPrice = leg->last();
		if(legPrice > 0)
		{
			if(leg->side() == entity::LONG)
			{
				diff +=	legPrice;
			}
			else
				diff -= legPrice;
		}
		else	// if one of legs has no price, set diff 0 anyway
			diff = 0;
	}

	m_innerItem->set_diff(diff);
	PushUpdate();
}

void CPortfolio::SetManager( CPortfolioManager* parentMgr )
{
	m_porfMgr = parentMgr;

	vector<string> listenSymbols;
	BOOST_FOREACH(const LegPtr& leg, m_vecLegs)
	{
		listenSymbols.push_back(leg->Symbol());
	}

	SetSymbols(listenSymbols);
	m_porfMgr->QuoteAggregator()->SubscribeQuotes(this);
}

void CPortfolio::Cleanup()
{
	m_porfMgr->QuoteAggregator()->UnsubscribeQuotes(this);
}

void CPortfolio::PushUpdate()
{
	m_porfMgr->PublishPortfolioUpdate(m_innerItem.get());
}
