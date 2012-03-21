#include "StdAfx.h"
#include "Portfolio.h"

#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>


CPortfolio::CPortfolio(void):
	m_quantity(0),
	m_portfolioID(boost::uuids::random_generator()())
{
}


CPortfolio::~CPortfolio(void)
{
}
