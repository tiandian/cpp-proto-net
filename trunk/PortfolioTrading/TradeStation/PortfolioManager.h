#pragma once

#include "OrderProcessor.h"

class CPortfolioManager
{
public:
	CPortfolioManager(void);
	~CPortfolioManager(void);

private:
	COrderProcessor m_orderProcessor;
};

