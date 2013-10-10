#pragma once
#include "Portfolio.h"
#include "entity/message.pb.h"

class CQuoteRepositry;
class CAvatarClient;

class CPortfolioManager
{
public:
	CPortfolioManager(void);
	~CPortfolioManager(void);

	void SetQuoteRepository(CQuoteRepositry* pQuoteRepository){ m_pQuoteRepositry = pQuoteRepository; }
	void AddPortfolio(CAvatarClient* client, const entity::PortfolioItem& portfItem);
	CPortfolio* Get(const string& portfolioId);
	void Clear(){  m_portfolios.clear(); }
private:
	std::vector<PortfolioPtr> m_portfolios;
	CQuoteRepositry* m_pQuoteRepositry;
};

