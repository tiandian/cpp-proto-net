#include "StdAfx.h"
#include "PortfolioUpdateItem.h"

namespace PTEntity {

StrategyUpdateItem^ CreateStrategyUpdate(StrategyType straType, entity::PortfolioUpdateItem* pEntity)
{
	switch(straType)
	{
	case StrategyType::ARBITRAGE:
		return gcnew ArbitrageStrategyUpdateItem
			(
				pEntity->ar_diff(),
				pEntity->ar_longdiff(),
				pEntity->ar_shortdiff(),
				pEntity->ar_longsize(),
				pEntity->ar_shortsize()
			);
	case StrategyType::CHANGE_POSITION:
		return gcnew ChangePosiStrategyUpdateItem
			(
				pEntity->ar_diff(),
				pEntity->ar_longdiff(),
				pEntity->ar_shortdiff(),
				pEntity->ar_longsize(),
				pEntity->ar_shortsize()
			);
	case StrategyType::SCALPER:
		return gcnew ScalperStrategyUpdateItem
			(
				pEntity->sc_diff()
			);
	default:
		return nullptr;
	}
}

PortfolioUpdateItem::PortfolioUpdateItem(entity::PortfolioUpdateItem* pEntity)
{
	_legs = gcnew List<LegUpdateItem^>();

	_id = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntity->id().c_str());

	_totalOpenTimes = pEntity->totalopentimes();
	_totalCloseTimes = pEntity->totalclosetimes();
	_currentPosition = pEntity->currentposition();
	_cancelTimes = pEntity->canceltimes();
	
	_strategyType = static_cast<StrategyType>(pEntity->strategy());

	for(int i = 0; i < pEntity->legs_size(); ++i)
	{
		const entity::LegUpdateItem& legUpdate = pEntity->legs(i);
		LegUpdateItem ^leg = gcnew LegUpdateItem(&legUpdate);
		_legs->Add(leg);
	}

	_strategyUpdate = CreateStrategyUpdate(_strategyType, pEntity);
}

}
