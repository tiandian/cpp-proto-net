#include "StdAfx.h"
#include "TriggerItem.h"


namespace PTEntity {

TriggerItem::TriggerItem(void)
{
}

void TriggerItem::To( entity::TriggerItem* pNativeTriggerItem )
{
	IntPtr namePointer;
	try
	{
		namePointer = (IntPtr)Marshal::StringToHGlobalAnsi(_name);
		pNativeTriggerItem->set_name((char*)namePointer.ToPointer());
		pNativeTriggerItem->set_strategy(static_cast<entity::StrategyType>(_strategy));
		pNativeTriggerItem->set_enabled(_enabled);
	}
	finally
	{
		Marshal::FreeHGlobal(namePointer);
	}
}


void ArbitrageTriggerItem::To( entity::TriggerItem* pNativeTriggerItem )
{
	pNativeTriggerItem->set_ar_condition(static_cast<entity::CompareCondition>(_condition));
	pNativeTriggerItem->set_ar_threshold(_threshold);
	pNativeTriggerItem->set_ar_offset(static_cast<entity::PosiOffsetFlag>(_offset));

	TriggerItem::To(pNativeTriggerItem);
}


void ChangePositionTriggerItem::To( entity::TriggerItem* pNativeTriggerItem )
{
	pNativeTriggerItem->set_cp_condition(static_cast<entity::CompareCondition>(_condition));
	pNativeTriggerItem->set_cp_threshold(_threshold);

	TriggerItem::To(pNativeTriggerItem);
}


void ScalperTriggerItem::To( entity::TriggerItem* pNativeTriggerItem )
{
	pNativeTriggerItem->set_sc_threshold(_threshold);

	TriggerItem::To(pNativeTriggerItem);
}


void HistSlopeTriggerItem::To( entity::TriggerItem* pNativeTriggerItem )
{
	pNativeTriggerItem->set_hs_offset(static_cast<entity::PosiOffsetFlag>(_offset));
	pNativeTriggerItem->set_hs_fastanglethreshold(_fastAngleThreshold);
	pNativeTriggerItem->set_hs_slowanglethreshold(_slowAngleThreshold);

	TriggerItem::To(pNativeTriggerItem);
}


void HistSlopeTrailingStopTriggerItem::To( entity::TriggerItem* pNativeTriggerItem )
{
	pNativeTriggerItem->set_ts_backvalue(_backValue);

	TriggerItem::To(pNativeTriggerItem);
}

void CrossTrailingStopTriggerItem::To( entity::TriggerItem* pNativeTriggerItem )
{
	pNativeTriggerItem->set_ts_backvalue(_backValue);

	TriggerItem::To(pNativeTriggerItem);
}

}
