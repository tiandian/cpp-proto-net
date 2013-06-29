#include "StdAfx.h"
#include "PortfolioItem.h"


void PTEntity::PortfolioItem::To( entity::PortfolioItem* pNativePortfolio )
{
	IntPtr idPointer;
	try
	{
		idPointer = (IntPtr)Marshal::StringToHGlobalAnsi(_id);
		pNativePortfolio->set_id((char*)idPointer.ToPointer());
		pNativePortfolio->set_quantity(_quantity);
		pNativePortfolio->set_maxcancel(_maxCancel);
		pNativePortfolio->set_maxtriggertimes(_maxTriggerTimes);

		for each(LegItem ^l in _legs)
		{
			entity::LegItem* pAddedLeg = pNativePortfolio->add_legs();
			l->To(pAddedLeg);
		}
		
		entity::StrategyItem* pStraItem = pNativePortfolio->mutable_strategy();
		if(_strategy != nullptr)
		{
			_strategy->To(pStraItem);
		}
	}
	finally
	{
		Marshal::FreeHGlobal(idPointer);
	}
}

void PTEntity::LegItem::To( entity::LegItem* pNativeLegItem )
{
	IntPtr symbolPointer;
	try
	{
		pNativeLegItem->set_legid(_legId);
		symbolPointer = (IntPtr)Marshal::StringToHGlobalAnsi(_symbol);
		pNativeLegItem->set_symbol((char*)symbolPointer.ToPointer());

		pNativeLegItem->set_ratio(_ratio);
		entity::PosiDirectionType convertedSide = static_cast<entity::PosiDirectionType>(_side);
		pNativeLegItem->set_side(convertedSide);
		pNativeLegItem->set_ispreferred(_isPreferred);
	}
	finally
	{
		Marshal::FreeHGlobal(symbolPointer);
	}
}
