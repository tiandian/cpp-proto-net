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

	void SetNote(const char* noteText){ m_strNote = noteText; }
	const string& GetNote(){ return m_strNote; }

	void SetDirection(entity::PosiDirectionType direction){ m_direction = direction; }
	entity::PosiDirectionType GetDirection(){ return m_direction; }

	entity::PosiOffsetFlag GetOffset(){ return m_offset; }

	void SetForceFire(bool val){ m_forceFire = val; }
	bool GetForceFire(){ return m_forceFire; }

	void Fire(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);

private:

	void OpenPosition(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);
	void ClosePosition(entity::Quote* pQuote, CPortfolio* pPortfolio, boost::chrono::steady_clock::time_point& timestamp);

	entity::PosiOffsetFlag m_offset;
	entity::PosiDirectionType m_direction;
	CPortfolioTrendOrderPlacer* m_pOrderPlacer;
	CTechAnalyStrategy* m_parentStrategy;
	bool m_forceFire;
	string m_strNote;
};

typedef boost::shared_ptr<CStrategyOrderCommand> OrderCommandPtr;

