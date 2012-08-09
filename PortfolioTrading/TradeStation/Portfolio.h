#pragma once

#include "QuoteListener.h"
#include "../Entity/gen/cpp/message.pb.h"

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>


using namespace std;

class CLeg;
class CPortfolioManager;

typedef boost::shared_ptr<entity::PortfolioItem> PortfItemPtr;
typedef boost::shared_ptr<CLeg> LegPtr;

class CPortfolio : public CQuoteListener
{
	CPortfolio(void);

public:
	~CPortfolio(void);

	static CPortfolio* Create(entity::PortfolioItem* pPortfItem)
	{
		CPortfolio* pPortf = new CPortfolio();
		pPortf->SetItem(pPortfItem);
		return pPortf;
	}

	void SetManager(CPortfolioManager* parentMgr);

	int Count();
	const string& ID(){ return m_innerItem->id(); }

	CLeg* AddLeg(entity::LegItem* pLegItem);

	CLeg* GetLeg(int legId);

	vector<LegPtr>& Legs(){ return m_vecLegs; }

	void OnQuoteRecevied(boost::shared_ptr<entity::Quote>& pQuote);

	// Need to be done before remove from manager
	void Cleanup();

	double Difference(){ return m_innerItem->diff(); }

private:
	void SetItem(entity::PortfolioItem* pPortfItem);

	void PushUpdate();

	vector<LegPtr> m_vecLegs;
	PortfItemPtr m_innerItem;

	CPortfolioManager* m_porfMgr;
};

class CLeg
{
	CLeg(int legId) : m_legId(legId)
	{}

public:
	~CLeg(){}

	friend CLeg* CPortfolio::AddLeg(entity::LegItem* pLegItem);

	int Id() { return m_legId; }

	const string& Symbol(){ return m_symbol; }
	void SetSymbol(const string& symb) { m_symbol = symb; }

	entity::LegItem* Item(){ return m_pInnerItem; }

private:
	void SetItem(entity::LegItem* pLegItem){ m_pInnerItem = pLegItem; }

	string m_symbol;
	int m_legId;
	entity::LegItem* m_pInnerItem;
};


