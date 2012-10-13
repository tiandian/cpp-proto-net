#include "StdAfx.h"
#include "ChangePositionStrategy.h"


CChangePositionStrategy::CChangePositionStrategy(void)
{
}


CChangePositionStrategy::~CChangePositionStrategy(void)
{
}

void CChangePositionStrategy::ApplySettings( const std::string& settingData )
{
	entity::ChangePosiStrategySettings settings;
	settings.ParseFromString(settingData);

	CloseLeg(settings.closeleg());
	SetOpenPosiCond(ConvertCompareCondition(settings.triggercondition()), 
											settings.threshold());

}
