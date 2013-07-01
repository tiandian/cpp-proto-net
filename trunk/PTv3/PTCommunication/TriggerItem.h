#pragma once

#include "entity/message.pb.h"
#include "Enums.h"

using namespace System;
using namespace System::Runtime::InteropServices;

namespace PTEntity {

public ref class TriggerItem
{
public:
	TriggerItem(void);

	virtual void To(entity::TriggerItem* pNativeTriggerItem);

private:
	StrategyType _strategy;
	String ^_name;
	bool _enabled;
};

public ref class ArbitrageTriggerItem : TriggerItem
{
public:
	ArbitrageTriggerItem(){}

	virtual void To(entity::TriggerItem* pNativeTriggerItem) override;

private:
	CompareCondition _condition;
	double _threshold;
	PosiOffsetFlag _offset;
};

public ref class ChangePositionTriggerItem : TriggerItem
{
public:
	ChangePositionTriggerItem(){}

	virtual void To(entity::TriggerItem* pNativeTriggerItem) override;

private:
	CompareCondition _condition;
	double _threshold;
};

public ref class ScalperTriggerItem : TriggerItem
{
public:
	ScalperTriggerItem(){}

	virtual void To(entity::TriggerItem* pNativeTriggerItem) override;

private:
	double _threshold;
};

}
