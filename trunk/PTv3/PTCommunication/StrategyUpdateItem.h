#pragma once

#include "Enums.h"

using namespace System;

namespace PTEntity {

public ref class TriggerStatusItem
{
public:
	TriggerStatusItem(String^ name, bool enabled)
	{
		_name = name;
		_enabled = enabled;
	}

	property String^ Name
	{
		String^ get()
		{
			return _name; 
		}
	}

	property bool Enabled
	{
		bool get()
		{
			return _enabled; 
		}
	}

private:
	String^ _name;
	bool _enabled;
};

public ref class StrategyUpdateItem abstract
{
public:
	StrategyUpdateItem(void){}

	property StrategyType Kind
	{
		virtual StrategyType get() abstract;
	}

	property bool Running
	{
		bool get()
		{
			return _running; 
		}
	}

	property array<TriggerStatusItem^>^ Triggers
	{
		array<TriggerStatusItem^>^ get()
		{
			return _triggers; 
		}
	}

internal:

	void SetTriggers(array<TriggerStatusItem^>^ triggers)
	{
		_triggers = triggers;
	}
	
	void SetRunning(bool val)
	{
		_running = val; 
	}

protected:
	bool _running;
	array<TriggerStatusItem^>^ _triggers;
};

public ref class ArbitrageStrategyUpdateItem : StrategyUpdateItem
{
public:
	ArbitrageStrategyUpdateItem(double diff, double longDiff, double shortDiff, int longSize, int shortSize, double bollTop, double bollBottom)
	{
		_diff = diff;
		_longDiff = longDiff;
		_shortDiff = shortDiff;
		_longSize = longSize;
		_shortSize = shortSize;

		_bollTop = bollTop;
		_bollBottom = bollBottom;
	}

	property StrategyType Kind
	{
		virtual StrategyType get() override
		{
			return StrategyType::ARBITRAGE;
		}
	}

	property double Diff
	{
		double get()
		{
			return _diff; 
		}
	}

	property double LongDiff
	{
		double get()
		{
			return _longDiff; 
		}
	}

	property double ShortDiff
	{
		double get()
		{
			return _shortDiff; 
		}
	}

	property int LongSize
	{
		int get()
		{
			return _longSize; 
		}
	}

	property int ShortSize
	{
		int get()
		{
			return _shortSize; 
		}
	}

	property double BollTop
	{
		double get()
		{
			return _bollTop; 
		}
		void set(double val)
		{
			_bollTop = val;
		}
	}

	property double BollBottom
	{
		double get()
		{
			return _bollBottom; 
		}
		void set(double val)
		{
			_bollBottom = val;
		}
	}
protected:
	double _diff;
	double _longDiff;
	double _shortDiff;
	int _longSize;
	int _shortSize;

	double _bollBottom;
	double _bollTop;
};

public ref class ChangePosiStrategyUpdateItem : ArbitrageStrategyUpdateItem
{
public:
	ChangePosiStrategyUpdateItem(double diff, double longDiff, double shortDiff, int longSize, int shortSize)
		: ArbitrageStrategyUpdateItem(diff, longDiff, shortDiff, longSize, shortSize, 0, 0) 
	{}

	property StrategyType Kind
	{
		virtual StrategyType get() override
		{
			return StrategyType::CHANGE_POSITION;
		}
	}
};

public ref class ManualStrategyUpdateItem : StrategyUpdateItem
{
public:
	ManualStrategyUpdateItem(double profit, double nearHigh, double nearLow, double fallback, double bounce)
	{
		_profit = profit;
		_nearHigh = nearHigh;
		_nearLow = nearLow;
		_fallback = fallback;
		_bounce = bounce;
	}

	property StrategyType Kind
	{
		virtual StrategyType get() override
		{
			return StrategyType::MANUAL;
		}
	}

	property double Profit
	{
		double get()
		{
			return _profit;
		}
	}

	property double NearHigh
	{
		double get()
		{
			return _nearHigh;
		}
	}

	property double NearLow
	{
		double get()
		{
			return _nearLow;
		}
	}

	property double Fallback
	{
		double get()
		{
			return _fallback;
		}
	}

	property double Bounce
	{
		double get()
		{
			return _bounce;
		}
	}

protected:
	double _profit;
	double _nearHigh;
	double _nearLow;
	double _fallback;
	double _bounce;
	
};

public ref class ScalperStrategyUpdateItem : StrategyUpdateItem
{
public:
	ScalperStrategyUpdateItem(double diff)
	{
		_diff = diff;
	}

	property StrategyType Kind
	{
		virtual StrategyType get() override
		{
			return StrategyType::SCALPER;
		}
	}

	property double Diff
	{
		double get()
		{
			return _diff; 
		}
		void set(double val)
		{
			_diff = val;
		}
	}

private:
	double _diff;
};

public ref class HistSlopeStrategyUpdateItem : StrategyUpdateItem
{
public:
	HistSlopeStrategyUpdateItem(double fastAngle, double slowAngle,
		double fastHistVal, double fastHistDiff, double slowHistVal, double slowHistDiff,
		SlopeDirection fastSlopeDirection, SlopeDirection slowSlopeDirection)
	{
		_fastAngle = fastAngle;
		_slowAngle = slowAngle;
		
		_fastHistVal = fastHistVal;
		_fastHistDiff = fastHistDiff;
		_slowHistVal = slowHistVal;
		_slowHistDiff = slowHistDiff;

		_fastSlopeDirection = fastSlopeDirection;
		_slowSlopeDirection = slowSlopeDirection;
		
	}

	property StrategyType Kind
	{
		virtual StrategyType get() override
		{
			return StrategyType::HIST_SLOPE;
		}
	}

	property double FastAngle
	{
		double get()
		{
			return _fastAngle; 
		}
		void set(double val)
		{
			_fastAngle = val;
		}
	}

	property double SlowAngle
	{
		double get()
		{
			return _slowAngle; 
		}
		void set(double val)
		{
			_slowAngle = val;
		}
	}

	property double FastMacdHist
	{
		double get()
		{
			return _fastHistVal; 
		}
		void set(double val)
		{
			_fastHistVal = val;
		}
	}

	property double FastMacdHistDiff
	{
		double get()
		{
			return _fastHistDiff; 
		}
		void set(double val)
		{
			_fastHistDiff = val;
		}
	}

	property double SlowMacdHist
	{
		double get()
		{
			return _slowHistVal; 
		}
		void set(double val)
		{
			_slowHistVal = val;
		}
	}

	property double SlowMacdHistDiff
	{
		double get()
		{
			return _slowHistDiff; 
		}
		void set(double val)
		{
			_slowHistDiff = val;
		}
	}

	property SlopeDirection FastMacdSlopeDirection
	{
		SlopeDirection get()
		{
			return _fastSlopeDirection; 
		}
		void set(SlopeDirection val)
		{
			_fastSlopeDirection = val;
		}
	}

	property SlopeDirection SlowMacdSlopeDirection
	{
		SlopeDirection get()
		{
			return _slowSlopeDirection; 
		}
		void set(SlopeDirection val)
		{
			_slowSlopeDirection = val;
		}
	}

private:
	double _fastAngle;
	double _slowAngle;

	double _fastHistVal;
	double _fastHistDiff;
	double _slowHistVal;
	double _slowHistDiff;

	SlopeDirection _fastSlopeDirection;
	SlopeDirection _slowSlopeDirection;
};

public ref class WMATrendStrategyUpdateItem : StrategyUpdateItem
{
public:
	WMATrendStrategyUpdateItem(double fastLine, double slowLine)
	{
		_fastLine = fastLine;
		_slowLine = slowLine;
	}

	property StrategyType Kind
	{
		virtual StrategyType get() override
		{
			return StrategyType::WMA_TREND;
		}
	}

	property double FastLine
	{
		double get()
		{
			return _fastLine; 
		}
		void set(double val)
		{
			_fastLine = val;
		}
	}

	property double SlowLine
	{
		double get()
		{
			return _slowLine; 
		}
		void set(double val)
		{
			_slowLine = val;
		}
	}

private:
	double _fastLine;
	double _slowLine;

};

public ref class LinerRegStrategyUpdateItem : StrategyUpdateItem
{
public:
	LinerRegStrategyUpdateItem(double weightMidPoint, double linerRegAngle)
	{
		_weightMidPoint = weightMidPoint;
		_linerRegAngle = linerRegAngle;
	}

	property StrategyType Kind
	{
		virtual StrategyType get() override
		{
			return StrategyType::LINER_REGRESSION;
		}
	}

	property double WeightMidPoint
	{
		double get()
		{
			return _weightMidPoint; 
		}
		void set(double val)
		{
			_weightMidPoint = val;
		}
	}

	property double LinerRegAngle
	{
		double get()
		{
			return _linerRegAngle; 
		}
		void set(double val)
		{
			_linerRegAngle = val;
		}
	}

private:
	double _weightMidPoint;
	double _linerRegAngle;
};

public ref class ASCTrendStrategyUpdateItem : StrategyUpdateItem
{
public:
	ASCTrendStrategyUpdateItem(double williamsR, double stopPx, double donchianHi, double donchianLo)
	{
		_williamsR = williamsR;
		_stopPx = stopPx;
		_donchianHi = donchianHi;
		_donchianLo = donchianLo;
	}

	property StrategyType Kind
	{
		virtual StrategyType get() override
		{
			return StrategyType::ASC_TREND;
		}
	}

	property double WilliamsR
	{
		double get()
		{
			return _williamsR; 
		}
		void set(double val)
		{
			_williamsR = val;
		}
	}

	property double StopPx
	{
		double get()
		{
			return _stopPx; 
		}
		void set(double val)
		{
			_stopPx = val;
		}
	}

	property double DonchianHi
	{
		double get()
		{
			return _donchianHi; 
		}
		void set(double val)
		{
			_donchianHi = val;
		}
	}

	property double DonchianLo
	{
		double get()
		{
			return _donchianLo; 
		}
		void set(double val)
		{
			_donchianLo = val;
		}
	}

private:
	double _williamsR;
	double _stopPx;
	double _donchianHi;
	double _donchianLo;
};

public ref class RangeTrendStrategyUpdateItem : StrategyUpdateItem
{
public:
	RangeTrendStrategyUpdateItem(double upperBoundOpen, double lowerBoundOpen, double upperBoundClose, double lowerBoundClose, double lastCostPx, double recentStopLossPx)
	{
		_upperBoundOpen = upperBoundOpen;
		_lowerBoundOpen = lowerBoundOpen;
		_upperBoundClose = upperBoundClose;
		_lowerBoundClose = lowerBoundClose;
		_lastCostPx = lastCostPx;
		_recentStopLossPx = recentStopLossPx;
	}

	property StrategyType Kind
	{
		virtual StrategyType get() override
		{
			return StrategyType::RANGE_TREND;
		}
	}

	property double UpperBoundOpen
	{
		double get()
		{
			return _upperBoundOpen; 
		}
		void set(double val)
		{
			_upperBoundOpen = val;
		}
	}

	property double LowerBoundOpen
	{
		double get()
		{
			return _lowerBoundOpen; 
		}
		void set(double val)
		{
			_lowerBoundOpen = val;
		}
	}

	property double UpperBoundClose
	{
		double get()
		{
			return _upperBoundClose; 
		}
		void set(double val)
		{
			_upperBoundClose = val;
		}
	}

	property double LowerBoundClose
	{
		double get()
		{
			return _lowerBoundClose; 
		}
		void set(double val)
		{
			_lowerBoundClose = val;
		}
	}

	property double LastCostPx
	{
		double get()
		{
			return _lastCostPx; 
		}
		void set(double val)
		{
			_lastCostPx = val;
		}
	}

	property double RecentStopLossPx
	{
		double get()
		{
			return _recentStopLossPx; 
		}
		void set(double val)
		{
			_recentStopLossPx = val;
		}
	}

private:
	double _upperBoundOpen;
	double _lowerBoundOpen;
	double _upperBoundClose;
	double _lowerBoundClose;
	double _lastCostPx;
	double _recentStopLossPx;
};

}