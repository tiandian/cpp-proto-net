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

	property int RetryTimes
	{
		int get()
		{
			return _retryTimes; 
		}
		void set(int val)
		{
			_retryTimes = val;
		}
	}

	property List<TriggerItem^>^ Triggers
	{
		List<TriggerItem^>^ get()
		{
			return _triggers;
		}	
	}

	property int OpenTimeout
	{
		int get()
		{
			return _openTimeout; 
		}
		void set(int val)
		{
			_openTimeout = val;
		}
	}

	virtual void To(entity::StrategyItem* pNativeStrategyItem);

protected:
	StrategyType _type;
	int _retryTimes;
	int _openTimeout;

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
	ScalperStrategyItem()
	{
		_type = StrategyType::SCALPER;
	}

	virtual void To(entity::StrategyItem* pNativeStrategyItem) override;

	property double PriceTick
	{
		double get()
		{
			return _priceTick;
		}
		void set(double val)
		{
			_priceTick = val;
		}
	}

	property DirectionDepends CaseLE2Tick
	{
		DirectionDepends get()
		{
			return _caseLE2Tick;
		}
		void set(DirectionDepends val)
		{
			_caseLE2Tick = val;
		}
	}

	property DirectionDepends CaseLE3Tick
	{
		DirectionDepends get()
		{
			return _caseLE3Tick; 
		}
		void set(DirectionDepends val)
		{
			_caseLE3Tick = val;
		}
	}

	property DirectionDepends CaseGE4Tick
	{
		DirectionDepends get()
		{
			return _caseGE4Tick; 
		}
		void set(DirectionDepends val)
		{
			_caseGE4Tick = val;
		}
	}

	property DirectionDepends CaseNoChange
	{
		DirectionDepends get()
		{
			return _caseNoChange; 
		}
		void set(DirectionDepends val)
		{
			_caseNoChange = val;
		}
	}

	property StopLossCloseMethods StopLossStrategy
	{
		StopLossCloseMethods get()
		{
			return _stopLossStrategy; 
		}
		void set(StopLossCloseMethods val)
		{
			_stopLossStrategy = val;
		}
	}

private:
	double _priceTick;
	DirectionDepends _caseLE2Tick;
	DirectionDepends _caseLE3Tick;
	DirectionDepends _caseGE4Tick;
	DirectionDepends _caseNoChange;
	StopLossCloseMethods _stopLossStrategy;
};

}