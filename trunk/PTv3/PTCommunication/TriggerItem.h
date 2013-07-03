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

protected:
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
	ScalperTriggerItem(double threshold)
	{
		_threshold = threshold;
		_name = gcnew String("ScalperTrigger");
	}

	virtual void To(entity::TriggerItem* pNativeTriggerItem) override;

	property double Threshold
	{
		double get()
		{
			return _threshold; 
		}
		void set(double val)
		{
			_threshold = val;
		}
	}

private:
	double _threshold;
};

}
