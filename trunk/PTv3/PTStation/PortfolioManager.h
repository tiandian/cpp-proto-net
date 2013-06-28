#pragma once
#include "Portfolio.h"
#include "entity/message.pb.h"

class CPortfolioManager
{
public:
	CPortfolioManager(void);
	~CPortfolioManager(void);

	void AddPortfolio(const entity::PortfolioItem& portfItem);

private:
	std::vector<PortfolioPtr> m_portfolios;
};

