#pragma once

#include "Enums.h"
#include "TriggerItem.h"
#include "entity/message.pb.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;

namespace PTEntity {

public ref class StrategyItem
{
public:
	StrategyItem(void);

	virtual void To(entity::StrategyItem* pNativeStrategyItem);

protected:
	StrategyType _type;
	bool _running;
	int _retryTimes;
	List<TriggerItem^> ^_triggers;
};

public ref class ArbitrageStrategyItem : StrategyItem
{
public:
	ArbitrageStrategyItem(){}

	virtual void To(entity::StrategyItem* pNativeStrategyItem) override;

private:
	PosiDirectionType _side;
};

public ref class ChangePositionStrategyItem : StrategyItem
{
public:
	ChangePositionStrategyItem(){}

	virtual void To(entity::StrategyItem* pNativeStrategyItem) override;

private:
	String ^_closeLeg;
	PosiDirectionType _closeLegSide;
};

public ref class ScalperStrategyItem : StrategyItem
{
public:
	ScalperStrategyItem(){}

	virtual void To(entity::StrategyItem* pNativeStrategyItem) override;

private:
	double _priceTick;
	DirectionDepends _caseLE2Tick;
	DirectionDepends _caseLE3Tick;
	DirectionDepends _caseGE4Tick;
	DirectionDepends _caseNoChange;
	int _openTimeout;
	StopLossCloseMethods _stopLossStrategy;
};

}