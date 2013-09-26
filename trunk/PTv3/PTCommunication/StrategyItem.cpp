#include "StdAfx.h"
#include "StrategyItem.h"

namespace PTEntity {

StrategyItem::StrategyItem(void)
{
	_triggers = gcnew List<TriggerItem^>();
}

void StrategyItem::To( entity::StrategyItem* pNativeStrategyItem )
{
	pNativeStrategyItem->set_type(static_cast<entity::StrategyType>(_type));
	pNativeStrategyItem->set_retrytimes(_retryTimes);
	pNativeStrategyItem->set_opentimeout(_openTimeout);

	for each(TriggerItem ^tr in _triggers)
	{
		entity::TriggerItem* pAddedTrigger = pNativeStrategyItem->add_triggers();
		tr->To(pAddedTrigger);
	}
}


void ArbitrageStrategyItem::To( entity::StrategyItem* pNativeStrategyItem )
{
	pNativeStrategyItem->set_ar_side(static_cast<entity::PosiDirectionType>(_side));

	StrategyItem::To(pNativeStrategyItem);
}


void ChangePositionStrategyItem::To( entity::StrategyItem* pNativeStrategyItem )
{
	IntPtr closeLegPointer;
	try
	{
		closeLegPointer = (IntPtr)Marshal::StringToHGlobalAnsi(_closeLeg);
		pNativeStrategyItem->set_cp_closeleg((char*)closeLegPointer.ToPointer());
		pNativeStrategyItem->set_cp_closelegside(static_cast<entity::PosiDirectionType>(_closeLegSide));

		StrategyItem::To(pNativeStrategyItem);
	}
	finally
	{
		Marshal::FreeHGlobal(closeLegPointer);
	}
}


void ScalperStrategyItem::To( entity::StrategyItem* pNativeStrategyItem )
{
	pNativeStrategyItem->set_sc_pricetick(_priceTick);
	pNativeStrategyItem->set_sc_casele2tick(static_cast<entity::DirectionDepends>(_caseLE2Tick));
	pNativeStrategyItem->set_sc_casele3tick(static_cast<entity::DirectionDepends>(_caseLE3Tick));
	pNativeStrategyItem->set_sc_casege4tick(static_cast<entity::DirectionDepends>(_caseGE4Tick));
	pNativeStrategyItem->set_sc_casenochange(static_cast<entity::DirectionDepends>(_caseNoChange));
	pNativeStrategyItem->set_sc_stoplossstrategy(static_cast<entity::StopLossCloseMethods>(_stopLossStrategy));

	StrategyItem::To(pNativeStrategyItem);
}


void MACDSlopeStrategyItem::To( entity::StrategyItem* pNativeStrategyItem )
{
	IntPtr symbolPtr;
	try
	{
		symbolPtr = (IntPtr)Marshal::StringToHGlobalAnsi(_symbol);

		entity::HistSourceCfg* fastSourceCfg = pNativeStrategyItem->add_histsources();
		fastSourceCfg->set_symbol((char*)symbolPtr.ToPointer());
		fastSourceCfg->set_precision(_fastPeriod);

		entity::HistSourceCfg* slowSourceCfg = pNativeStrategyItem->add_histsources();
		slowSourceCfg->set_symbol((char*)symbolPtr.ToPointer());
		slowSourceCfg->set_precision(_slowPeriod);

		pNativeStrategyItem->set_hs_short(_short);
		pNativeStrategyItem->set_hs_long(_long);
		pNativeStrategyItem->set_hs_m(_m);
		pNativeStrategyItem->set_hs_fastperiod(_fastPeriod);
		pNativeStrategyItem->set_hs_slowperiod(_slowPeriod);
		pNativeStrategyItem->set_hs_faststddiff(_fastStdDiff);
		pNativeStrategyItem->set_hs_slowstddiff(_slowStdDiff);

		pNativeStrategyItem->set_hs_fastshortemaseed(_fastShortSeed);
		pNativeStrategyItem->set_hs_fastlongemaseed(_fastLongSeed);
		pNativeStrategyItem->set_hs_fastsignalemaseed(_fastSignalSeed);

		pNativeStrategyItem->set_hs_slowshortemaseed(_slowShortSeed);
		pNativeStrategyItem->set_hs_slowlongemaseed(_slowLongSeed);
		pNativeStrategyItem->set_hs_slowsignalemaseed(_slowSignalSeed);

		StrategyItem::To(pNativeStrategyItem);
	}
	finally
	{
		Marshal::FreeHGlobal(symbolPtr);
	}
}

}
