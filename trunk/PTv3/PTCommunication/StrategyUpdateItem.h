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
	ArbitrageStrategyUpdateItem(double diff, double longDiff, double shortDiff, int longSize, int shortSize)
	{
		_diff = diff;
		_longDiff = longDiff;
		_shortDiff = shortDiff;
		_longSize = longSize;
		_shortSize = shortSize;
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
protected:
	double _diff;
	double _longDiff;
	double _shortDiff;
	int _longSize;
	int _shortSize;
};

public ref class ChangePosiStrategyUpdateItem : ArbitrageStrategyUpdateItem
{
public:
	ChangePosiStrategyUpdateItem(double diff, double longDiff, double shortDiff, int longSize, int shortSize)
		: ArbitrageStrategyUpdateItem(diff, longDiff, shortDiff, longSize, shortSize) 
	{}

	property StrategyType Kind
	{
		virtual StrategyType get() override
		{
			return StrategyType::CHANGE_POSITION;
		}
	}
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

public ref class CrossStrategyUpdateItem : StrategyUpdateItem
{
public:
	CrossStrategyUpdateItem(double fastHistVal, double slowHistVal, double bollTop, double bollBottom)
	{
		_fastHistVal = fastHistVal;
		_slowHistVal = slowHistVal;

		_bollTop = bollTop;
		_bollBottom = bollBottom;

	}

	property StrategyType Kind
	{
		virtual StrategyType get() override
		{
			return StrategyType::DOUBLE_CROSS;
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

private:
	double _fastHistVal;
	double _slowHistVal;

	double _bollTop;
	double _bollBottom;
};

}