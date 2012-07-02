#pragma once

#include "Portfolio.h"

#include <string>
#include <map>
#include <boost/shared_ptr.hpp>

using namespace std;

typedef boost::shared_ptr<CPortfolio> PortfolioPtr;

class CPortfolioManager
{
public:
	CPortfolioManager(void);
	~CPortfolioManager(void);

	void Add(CPortfolio* portfolio);
	CPortfolio* Get(const string& portfId);
	void Remove(const string& portfId);

private:

	typedef map<string, PortfolioPtr> PortfolioMap;
	typedef PortfolioMap::iterator PortfolioMapIter;

	PortfolioMap m_mapPortfolios;
	
};

