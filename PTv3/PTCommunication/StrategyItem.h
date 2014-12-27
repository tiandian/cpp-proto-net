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
	ArbitrageStrategyItem()
	{
		_type = StrategyType::ARBITRAGE;
		_bollPeriod = 26;
		_stdDevMultiplier = 2;
	}

	virtual void To(entity::StrategyItem* pNativeStrategyItem) override;

	property double BollPeriod
	{
		double get()
		{
			return _bollPeriod; 
		}
		void set(double val)
		{
			_bollPeriod = val;
		}
	}

	property double StdDevMultiplier
	{
		double get()
		{
			return _stdDevMultiplier; 
		}
		void set(double val)
		{
			_stdDevMultiplier = val;
		}
	}

	property String^ FirstLegSymbol
	{
		String^ get()
		{
			return _firstLegSymbol; 
		}
		void set(String^ val)
		{
			_firstLegSymbol = val;
		}
	}

	property String^ SecondLegSymbol
	{
		String^ get()
		{
			return _secondLegSymbol; 
		}
		void set(String^ val)
		{
			_secondLegSymbol = val;
		}
	}

	property int TimeFrame
	{
		int get()
		{
			return _timeFrame; 
		}
		void set(int val)
		{
			_timeFrame = val;
		}
	}

private:

	String^ _firstLegSymbol;
	String^ _secondLegSymbol;
	int _timeFrame;

	double _bollPeriod;
	double _stdDevMultiplier;
};

public ref class ChangePositionStrategyItem : StrategyItem
{
public:
	ChangePositionStrategyItem()
	{
		_type = StrategyType::CHANGE_POSITION;
	}

	virtual void To(entity::StrategyItem* pNativeStrategyItem) override;

private:
	String ^_closeLeg;
	PosiDirectionType _closeLegSide;
};

public ref class ManualStrategyItem : StrategyItem
{
public:
	ManualStrategyItem()
	{
		_type = StrategyType::MANUAL;
	}

	virtual void To(entity::StrategyItem* pNativeStrategyItem) override;

	property PosiDirectionType Direction
	{
		PosiDirectionType get()
		{
			return _direction;
		}
		void set(PosiDirectionType val)
		{
			_direction = val;
		}
	}

	property CompareCondition StopGainCondition
	{
		CompareCondition get()
		{
			return _stopGainCondition;
		}
		void set(CompareCondition val)
		{
			_stopGainCondition = val;
		}
	}

	property double StopGainThreshold
	{
		double get()
		{
			return _stopGainThreshold;
		}
		void set(double val)
		{
			_stopGainThreshold = val;
		}
	}

	property CompareCondition StopLossCondition
	{
		CompareCondition get()
		{
			return _stopLossCondition;
		}
		void set(CompareCondition val)
		{
			_stopLossCondition = val;
		}
	}

	property double StopLossThreshold
	{
		double get()
		{
			return _stopLossThreshold;
		}
		void set(double val)
		{
			_stopLossThreshold = val;
		}
	}

	property StopPriceType StopLossType
	{
		StopPriceType get()
		{
			return _stopLossType;
		}
		void set(StopPriceType val)
		{
			_stopLossType = val;
		}
	}

private:
	PosiDirectionType _direction;
	CompareCondition _stopGainCondition;
	double _stopGainThreshold;
	CompareCondition _stopLossCondition;
	double _stopLossThreshold;
	StopPriceType _stopLossType;
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

public ref class WMATrendStrategyItem : StrategyItem
{
public:
	WMATrendStrategyItem()
	{
		_type = StrategyType::WMA_TREND;
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

	property int WMA_Param
	{
		int get()
		{
			return _wmaP; 
		}
		void set(int val)
		{
			_wmaP = val;
		}
	}

	property int MA_N
	{
		int get()
		{
			return _maN; 
		}
		void set(int val)
		{
			_maN = val;
		}
	}

	property int Period
	{
		int get()
		{
			return _period; 
		}
		void set(int val)
		{
			_period = val;
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

private:
	int _wmaP;
	int _maN;
	int _period;
	double _trailingStopValue;

	String^ _symbol;
};

public ref class LinerRegressionStrategyItem : StrategyItem
{
public:
	LinerRegressionStrategyItem()
	{
		_type = StrategyType::LINER_REGRESSION;
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

	property int Period
	{
		int get()
		{
			return _period; 
		}
		void set(int val)
		{
			_period = val;
		}
	}

	property int Number
	{
		int get()
		{
			return _number; 
		}
		void set(int val)
		{
			_number = val;
		}
	}

	property double OpenThreshold
	{
		double get()
		{
			return _openThreshold; 
		}
		void set(double val)
		{
			_openThreshold = val;
		}
	}

	property double CloseThreshold
	{
		double get()
		{
			return _closeThreshold; 
		}
		void set(double val)
		{
			_closeThreshold = val;
		}
	}

private:
	int _period;
	int _number;
	double _openThreshold;
	double _closeThreshold;

	String^ _symbol;
};

public ref class ASCTrendStrategyItem : StrategyItem
{
public:
	ASCTrendStrategyItem()
	{
		_type = StrategyType::ASC_TREND;
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

	property int Period
	{
		int get()
		{
			return _period; 
		}
		void set(int val)
		{
			_period = val;
		}
	}

	property int Risk
	{
		int get()
		{
			return _risk; 
		}
		void set(int val)
		{
			_risk = val;
		}
	}

	property int AveragePeriod
	{
		int get()
		{
			return _avgPeriod; 
		}
		void set(int val)
		{
			_avgPeriod = val;
		}
	}

	property int BreakoutLength
	{
		int get()
		{
			return _breakoutLength; 
		}
		void set(int val)
		{
			_breakoutLength = val;
		}
	}

private:
	int _risk;
	int _avgPeriod;
	int _period;
	int _breakoutLength;

	String^ _symbol;
};

public ref class RangeTrendStrategyItem : StrategyItem
{
public:
	RangeTrendStrategyItem()
	{
		_type = StrategyType::RANGE_TREND;
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

	property int Period
	{
		int get()
		{
			return _period; 
		}
		void set(int val)
		{
			_period = val;
		}
	}

	property int OpenPeriod
	{
		int get()
		{
			return _openPeriod; 
		}
		void set(int val)
		{
			_openPeriod = val;
		}
	}

	property int ClosePeriod
	{
		int get()
		{
			return _closePeriod; 
		}
		void set(int val)
		{
			_closePeriod = val;
		}
	}

	property double StopLossFactor
	{
		double get()
		{
			return _stopLossFactor; 
		}
		void set(double val)
		{
			_stopLossFactor = val;
		}
	}

	property double TrendFactor
	{
		double get()
		{
			return _trendFactor; 
		}
		void set(double val)
		{
			_trendFactor = val;
		}
	}

private:

	int _openPeriod;
	int _closePeriod;
	double _stopLossFactor;
	double _trendFactor;

	int _period;
	String^ _symbol;
};

}