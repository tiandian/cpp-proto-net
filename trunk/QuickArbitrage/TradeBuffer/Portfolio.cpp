#include "StdAfx.h"
#include "Portfolio.h"

#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/foreach.hpp>

CPortfolio::CPortfolio(void):
	m_quantity(0),
	m_portfolioID(boost::uuids::random_generator()())
{
}

CPortfolio::CPortfolio(const boost::uuids::uuid& pid):
	m_quantity(0),
	m_portfolioID(pid)
{
}

CPortfolio::~CPortfolio(void)
{
}

double CPortfolio::GetDiff()
{
	double diff = 0;
	BOOST_FOREACH(const boost::shared_ptr<CLeg>& leg, m_legs)
	{
		protoc::PosiDirectionType side = leg->GetSide();
		if(side == protoc::LONG)
		{
			diff += leg->GetCost();
		}
		else if(side == protoc::SHORT)
		{
			diff -= leg->GetCost();
		}
	}
	return diff;
}
