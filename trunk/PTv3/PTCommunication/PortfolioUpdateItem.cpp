#include "StdAfx.h"
#include "PortfolioUpdateItem.h"
#include "charsetconvert.h"

namespace PTEntity {

StrategyUpdateItem^ CreateStrategyUpdate(StrategyType straType, entity::PortfolioUpdateItem* pEntity)
{
	StrategyUpdateItem^ retStrategyUpdate = nullptr;
	switch(straType)
	{
	case StrategyType::ARBITRAGE:
		retStrategyUpdate = gcnew ArbitrageStrategyUpdateItem
			(
				pEntity->ar_diff(),
				pEntity->ar_longdiff(),
				pEntity->ar_shortdiff(),
				pEntity->ar_longsize(),
				pEntity->ar_shortsize()
			);
		break;
	case StrategyType::CHANGE_POSITION:
		retStrategyUpdate = gcnew ChangePosiStrategyUpdateItem
			(
				pEntity->ar_diff(),
				pEntity->ar_longdiff(),
				pEntity->ar_shortdiff(),
				pEntity->ar_longsize(),
				pEntity->ar_shortsize()
			);
		break;
	case StrategyType::SCALPER:
		retStrategyUpdate = gcnew ScalperStrategyUpdateItem
			(
				pEntity->sc_diff()
			);
		break;
	case StrategyType::HIST_SLOPE:
		retStrategyUpdate = gcnew HistSlopeStrategyUpdateItem
			(
				pEntity->hs_fastangle(),
				pEntity->hs_slowangle(),
				pEntity->hs_fastmacdhist(),
				pEntity->hs_fastmacdhistdiff(),
				pEntity->hs_slowmacdhist(),
				pEntity->hs_slowmacdhistdiff(),
				static_cast<SlopeDirection>(pEntity->hs_fastslopedirection()),
				static_cast<SlopeDirection>(pEntity->hs_slowslopedirection())
			);
		break;
	default:
		return nullptr;
	}

	retStrategyUpdate->SetRunning(pEntity->running());

	List<TriggerStatusItem^> ^triggerList = gcnew List<TriggerStatusItem^>();
	for(int i = 0; i < pEntity->triggers_size(); ++i)
	{
		const entity::TriggerStatus& triggerEntity = pEntity->triggers(i);
		String ^triggerName = Marshal::PtrToStringAnsi((IntPtr) (char *) triggerEntity.name().c_str());
		TriggerStatusItem ^ triggerStatus = gcnew TriggerStatusItem(triggerName, triggerEntity.enabled());
		triggerList->Add(triggerStatus);
	}

	retStrategyUpdate->SetTriggers(triggerList->ToArray());

	return retStrategyUpdate;
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

	_hasMsg = pEntity->has_message();
	if(_hasMsg)
	{
		wchar_t* uniStr = UTF8ToUnicode(pEntity->message().c_str());
		_msg = Marshal::PtrToStringAuto((IntPtr)uniStr);
		delete[] uniStr;
	}

}

}
