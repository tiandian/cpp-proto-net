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
		init();
	}
	ScalperTriggerItem(double threshold)
	{
		init();
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
	void init()
	{
		_strategy = StrategyType::SCALPER;
		_name = gcnew String("ScalperTrigger");
	}

	double _threshold;
};

public ref class HistSlopeTriggerItem : TriggerItem
{
public:
	HistSlopeTriggerItem()
	{
		init();
	}

	virtual void To(entity::TriggerItem* pNativeTriggerItem) override;

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

	property double FastAngleThreshold
	{
		double get()
		{
			return _fastAngleThreshold; 
		}
		void set(double val)
		{
			_fastAngleThreshold = val;
		}
	}

	property double SlowAngleThreshold
	{
		double get()
		{
			return _slowAngleThreshold; 
		}
		void set(double val)
		{
			_slowAngleThreshold = val;
		}
	}

private:
	void init()
	{
		_strategy = StrategyType::HIST_SLOPE;
		_name = gcnew String("HistSlopeTrigger");
	}

	PosiOffsetFlag _offset;
	double _fastAngleThreshold;
	double _slowAngleThreshold;

};

public ref class HistSlopeTrailingStopTriggerItem : TriggerItem
{
public:
	HistSlopeTrailingStopTriggerItem()
	{
		init();
	}
	HistSlopeTrailingStopTriggerItem(double backValue)
	{
		init();
		_backValue = backValue;
	}

	virtual void To(entity::TriggerItem* pNativeTriggerItem) override;

	property double BackValue
	{
		double get()
		{
			return _backValue; 
		}
		void set(double val)
		{
			_backValue = val;
		}
	}

private:
	void init()
	{
		_strategy = StrategyType::HIST_SLOPE;
		_name = gcnew String("HistSlopeTrailingStopTrigger");
	}

	double _backValue;
};

public ref class CrossOpenTriggerItem : TriggerItem
{
public:
	CrossOpenTriggerItem()
	{
		init();
	}

	property PosiOffsetFlag Offset
	{
		PosiOffsetFlag get()
		{
			return PosiOffsetFlag::OPEN; 
		}
	}

private:
	void init()
	{
		_strategy = StrategyType::DOUBLE_CROSS;
		_name = gcnew String("MACDCrossOpenTrigger");
	}

};

public ref class CrossCloseTriggerItem : TriggerItem
{
public:
	CrossCloseTriggerItem()
	{
		init();
	}

	property PosiOffsetFlag Offset
	{
		PosiOffsetFlag get()
		{
			return PosiOffsetFlag::CLOSE; 
		}
	}

private:
	void init()
	{
		_strategy = StrategyType::DOUBLE_CROSS;
		_name = gcnew String("MACDCrossCloseTrigger");
	}

};

public ref class CrossTrailingStopTriggerItem : TriggerItem
{
public:
	CrossTrailingStopTriggerItem()
	{
		init();
	}
	CrossTrailingStopTriggerItem(double backValue)
	{
		init();
		_backValue = backValue;
	}

	virtual void To(entity::TriggerItem* pNativeTriggerItem) override;

	property double BackValue
	{
		double get()
		{
			return _backValue; 
		}
		void set(double val)
		{
			_backValue = val;
		}
	}

private:
	void init()
	{
		_strategy = StrategyType::DOUBLE_CROSS;
		_name = gcnew String("MACDCrossTrailingStopTrigger");
	}

	double _backValue;
};

}
