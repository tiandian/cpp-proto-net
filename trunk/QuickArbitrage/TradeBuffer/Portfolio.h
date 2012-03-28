#pragma once

#include "Leg.h"

#include <vector>
#include <string>
#include <boost/smart_ptr.hpp>
#include <boost/uuid/uuid.hpp>

typedef std::vector< boost::shared_ptr<CLeg> > LegVector;

class CPortfolio
{
public:
	CPortfolio(void);
	CPortfolio(const boost::uuids::uuid& pid);
	~CPortfolio(void);

	const LegVector& GetLegs(){ return m_legs; }
	CLeg* AddLeg()
	{
		boost::shared_ptr<CLeg> leg(new CLeg(this));
		m_legs.push_back(leg);
		return leg.get();
	}

	int GetQuantity(){ return m_quantity; }
	void SetQuantity(int quantity){ m_quantity = quantity; }

	double GetDiff();

	const boost::uuids::uuid& GetID() {return m_portfolioID;}

private:
	
	LegVector m_legs;
	int m_quantity;

	boost::uuids::uuid m_portfolioID;
};

