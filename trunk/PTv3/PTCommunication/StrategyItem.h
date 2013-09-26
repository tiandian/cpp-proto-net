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

public ref class MACDSlopeStrategyItem : StrategyItem
{
public:
	MACDSlopeStrategyItem()
	{
		_type = StrategyType::HIST_SLOPE;
	}

	virtual void To(entity::StrategyItem* pNativeStrategyItem) override;

	property String^ Symbol
	{
		String^ get()
		{
			return _symbol; 
		}
		void set(String^ val)
		{
			_symbol = val;
		}
	}

	property int Short
	{
		int get()
		{
			return _short; 
		}
		void set(int val)
		{
			_short = val;
		}
	}

	property int Long
	{
		int get()
		{
			return _long; 
		}
		void set(int val)
		{
			_long = val;
		}
	}

	property int M
	{
		int get()
		{
			return _m; 
		}
		void set(int val)
		{
			_m = val;
		}
	}

	property int FastPeriod
	{
		int get()
		{
			return _fastPeriod; 
		}
		void set(int val)
		{
			_fastPeriod = val;
		}
	}

	property double FastStdDiff
	{
		double get()
		{
			return _fastStdDiff; 
		}
		void set(double val)
		{
			_fastStdDiff = val;
		}
	}

	property int FastAngleThreshold
	{
		int get()
		{
			return _fastAngleThreshold; 
		}
		void set(int val)
		{
			_fastAngleThreshold = val;
		}
	}

	property int SlowPeriod
	{
		int get()
		{
			return _slowPeriod; 
		}
		void set(int val)
		{
			_slowPeriod = val;
		}
	}

	property double SlowStdDiff
	{
		double get()
		{
			return _slowStdDiff; 
		}
		void set(double val)
		{
			_slowStdDiff = val;
		}
	}

	property int SlowAngleThreshold
	{
		int get()
		{
			return _slowAngleThreshold; 
		}
		void set(int val)
		{
			_slowAngleThreshold = val;
		}
	}

	property double TrailingStopValue
	{
		double get()
		{
			return _trailingStopValue; 
		}
		void set(double val)
		{
			_trailingStopValue = val;
		}
	}

	property double FastShortSeed
	{
		double get()
		{
			return _fastShortSeed; 
		}
		void set(double val)
		{
			_fastShortSeed = val;
		}
	}

	property double FastLongSeed
	{
		double get()
		{
			return _fastLongSeed; 
		}
		void set(double val)
		{
			_fastLongSeed = val;
		}
	}

	property double FastSignalSeed
	{
		double get()
		{
			return _fastSignalSeed; 
		}
		void set(double val)
		{
			_fastSignalSeed = val;
		}
	}

	property double SlowShortSeed
	{
		double get()
		{
			return _slowShortSeed; 
		}
		void set(double val)
		{
			_slowShortSeed = val;
		}
	}

	property double SlowLongSeed
	{
		double get()
		{
			return _slowLongSeed; 
		}
		void set(double val)
		{
			_slowLongSeed = val;
		}
	}

	property double SlowSignalSeed
	{
		double get()
		{
			return _slowSignalSeed; 
		}
		void set(double val)
		{
			_slowSignalSeed = val;
		}
	}

private:
	int _short;
	int _long;
	int _m;
	int _fastPeriod;
	double _fastStdDiff;
	int _fastAngleThreshold;
	int _slowPeriod;
	double _slowStdDiff;
	int _slowAngleThreshold;
	double _trailingStopValue;

	double _fastShortSeed;
	double _fastLongSeed;
	double _fastSignalSeed;
	double _slowShortSeed;
	double _slowLongSeed;
	double _slowSignalSeed;

	String^ _symbol;
};

}