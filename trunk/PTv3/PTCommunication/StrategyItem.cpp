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
	IntPtr firstLegPtr;
	IntPtr secondLegPtr;
	try
	{
		firstLegPtr = (IntPtr)Marshal::StringToHGlobalAnsi(_firstLegSymbol);
		secondLegPtr = (IntPtr)Marshal::StringToHGlobalAnsi(_secondLegSymbol);

		pNativeStrategyItem->set_ar_bollperiod(_bollPeriod);
		pNativeStrategyItem->set_ar_stddevmultiplier(_stdDevMultiplier);

		entity::HistSourceCfg* firstLegSourceCfg = pNativeStrategyItem->add_histsources();
		firstLegSourceCfg->set_symbol((char*)firstLegPtr.ToPointer());
		firstLegSourceCfg->set_precision(_timeFrame);

		entity::HistSourceCfg* secondLegSourceCfg = pNativeStrategyItem->add_histsources();
		secondLegSourceCfg->set_symbol((char*)secondLegPtr.ToPointer());
		secondLegSourceCfg->set_precision(_timeFrame);

		StrategyItem::To(pNativeStrategyItem);
	}
	finally
	{
		Marshal::FreeHGlobal(firstLegPtr);
		Marshal::FreeHGlobal(secondLegPtr);
	}
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

void ManualStrategyItem::To(entity::StrategyItem* pNativeStrategyItem)
{
	pNativeStrategyItem->set_positiondirection(static_cast<entity::PosiDirectionType>(_direction));
	pNativeStrategyItem->set_stopgaincondition(static_cast<entity::CompareCondition>(_stopGainCondition));
	pNativeStrategyItem->set_stopgainthreshold(_stopGainThreshold);
	pNativeStrategyItem->set_stoplosscondition(static_cast<entity::CompareCondition>(_stopLossCondition));
	pNativeStrategyItem->set_stoplossthreshold(_stopLossThreshold);
	pNativeStrategyItem->set_stoplosstype(static_cast<entity::StopPriceType>(_stopLossType));

	StrategyItem::To(pNativeStrategyItem);
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

void WMATrendStrategyItem::To( entity::StrategyItem* pNativeStrategyItem )
{
	IntPtr symbolPtr;
	try
	{
		symbolPtr = (IntPtr)Marshal::StringToHGlobalAnsi(_symbol);

		entity::HistSourceCfg* fastSourceCfg = pNativeStrategyItem->add_histsources();
		fastSourceCfg->set_symbol((char*)symbolPtr.ToPointer());
		fastSourceCfg->set_precision(_period);

		pNativeStrategyItem->set_wt_wmaparam(_wmaP);
		pNativeStrategyItem->set_wt_man(_maN);
		pNativeStrategyItem->set_wt_period(_period);

		StrategyItem::To(pNativeStrategyItem);
	}
	finally
	{
		Marshal::FreeHGlobal(symbolPtr);
	}
}

void LinerRegressionStrategyItem::To( entity::StrategyItem* pNativeStrategyItem )
{
	IntPtr symbolPtr;
	try
	{
		symbolPtr = (IntPtr)Marshal::StringToHGlobalAnsi(_symbol);

		entity::HistSourceCfg* fastSourceCfg = pNativeStrategyItem->add_histsources();
		fastSourceCfg->set_symbol((char*)symbolPtr.ToPointer());
		fastSourceCfg->set_precision(_period);

		pNativeStrategyItem->set_lr_period(_period);
		pNativeStrategyItem->set_lr_number(_number);
		pNativeStrategyItem->set_lr_openthreshold(_openThreshold);
		pNativeStrategyItem->set_lr_closethreshold(_closeThreshold);

		StrategyItem::To(pNativeStrategyItem);
	}
	finally
	{
		Marshal::FreeHGlobal(symbolPtr);
	}
}


void ASCTrendStrategyItem::To( entity::StrategyItem* pNativeStrategyItem )
{
	IntPtr symbolPtr;
	try
	{
		symbolPtr = (IntPtr)Marshal::StringToHGlobalAnsi(_symbol);

		entity::HistSourceCfg* fastSourceCfg = pNativeStrategyItem->add_histsources();
		fastSourceCfg->set_symbol((char*)symbolPtr.ToPointer());
		fastSourceCfg->set_precision(_period);

		pNativeStrategyItem->set_as_risk(_risk);
		pNativeStrategyItem->set_as_avergateperiod(_avgPeriod);
		pNativeStrategyItem->set_as_breakoutlength(_breakoutLength);
		pNativeStrategyItem->set_as_period(_period);

		StrategyItem::To(pNativeStrategyItem);
	}
	finally
	{
		Marshal::FreeHGlobal(symbolPtr);
	}
}

void RangeTrendStrategyItem::To( entity::StrategyItem* pNativeStrategyItem )
{
	IntPtr symbolPtr;
	try
	{
		symbolPtr = (IntPtr)Marshal::StringToHGlobalAnsi(_symbol);

		entity::HistSourceCfg* fastSourceCfg = pNativeStrategyItem->add_histsources();
		fastSourceCfg->set_symbol((char*)symbolPtr.ToPointer());
		fastSourceCfg->set_precision(_period);

		pNativeStrategyItem->set_rt_timeframe(_period);
		pNativeStrategyItem->set_rt_openperiod(_openPeriod);
		pNativeStrategyItem->set_rt_closeperiod(_closePeriod);
		pNativeStrategyItem->set_rt_stoplossfactor(_stopLossFactor);
		pNativeStrategyItem->set_rt_trendfactor(_trendFactor);

		StrategyItem::To(pNativeStrategyItem);
	}
	finally
	{
		Marshal::FreeHGlobal(symbolPtr);
	}
}

}
