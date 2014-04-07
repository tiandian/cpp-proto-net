#pragma once

#include "entity/message.pb.h"
#include "entity/quote.pb.h"

class CPortfolioTrendOrderPlacer;
class CTechAnalyStrategy;
class CPortfolio;

class CStrategyOrderCommand
{
public:
	CStrategyOrderCommand(entity::PosiOffsetFlag offset, CPortfolioTrendOrderPlacer* pOrderPlacer, CTechAnalyStrategy* parentStrategy);
	~CStrategyOrderCommand(void);

	void SetNote(const string& noteText){ m_strNote = noteText; }
	const string& GetNote(){ return m_strNote; }

	void SetDirection(entity::PosiDirectionType direction){ m_direction = direction; }
	entity::PosiDirectionType GetDirection(){ return m_direction; }

	entity::PosiOffsetFlag GetOffset(){ return m_offset; }

	double Fire(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);

	bool IsActive(){ return m_active; }
	void Activate(){ m_active = true; }
	void Deactivate(){ m_active = false; }

	void Revert(entity::PosiDirectionType direction);
	void SetRevertOnClose(bool revert){ m_revertOnClose = revert; }
	bool GetRevertOnClose(){ return m_revertOnClose; }
private:

	double OpenPosition(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);
	double ClosePosition(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);

	entity::PosiOffsetFlag m_offset;
	entity::PosiDirectionType m_direction;
	CPortfolioTrendOrderPlacer* m_pOrderPlacer;
	CTechAnalyStrategy* m_parentStrategy;
	string m_strNote;
	bool m_active;
	bool m_revertOnClose;
};

typedef boost::shared_ptr<CStrategyOrderCommand> OrderCommandPtr;

