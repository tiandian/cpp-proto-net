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

	property String^ Name
	{
		String^ get()
		{
			return _name; 
		}
		void set(String^ val)
		{
			_name = val;
		}
	}

	property bool Enabled
	{
		bool get()
		{
			return _enabled; 
		}
		void set(bool val)
		{
			_enabled = val;
		}
	}

	virtual void To(entity::TriggerItem* pNativeTriggerItem);

protected:
	StrategyType _strategy;
	String ^_name;
	bool _enabled;
};

public ref class ArbitrageTriggerItem : TriggerItem
{
public:
	ArbitrageTriggerItem()
	{
		_strategy = StrategyType::ARBITRAGE;
		_name = gcnew String("ArbitrageTrigger");
	}

	property CompareCondition Condition
	{
		CompareCondition get()
		{
			return _condition; 
		}
		void set(CompareCondition val)
		{
			_condition = val;
		}
	}

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

	property PosiOffsetFlag Offset
	{
		PosiOffsetFlag get()
		{
			return _offset; 
		}
		void set(PosiOffsetFlag val)
		{
			_offset = val;
		}
	}

	virtual void To(entity::TriggerItem* pNativeTriggerItem) override;

private:
	CompareCondition _condition;
	double _threshold;
	PosiOffsetFlag _offset;
};

public ref class ChangePositionTriggerItem : TriggerItem
{
public:
	ChangePositionTriggerItem()
	{
		_strategy = StrategyType::CHANGE_POSITION;
		_name = gcnew String("ChangePosiTrigger");
	}

	property CompareCondition Condition
	{
		CompareCondition get()
		{
			return _condition; 
		}
		void set(CompareCondition val)
		{
			_condition = val;
		}
	}

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

	virtual void To(entity::TriggerItem* pNativeTriggerItem) override;

private:
	CompareCondition _condition;
	double _threshold;
};

public ref class ScalperTriggerItem : TriggerItem
{
public:
	ScalperTriggerItem()
	{
		_strategy = StrategyType::SCALPER;
		_name = gcnew String("ScalperTrigger");
	}
	ScalperTriggerItem(double threshold)
	{
		ScalperTriggerItem();
		_threshold = threshold;
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
