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
	HistSlopeStrategyUpdateItem(double fastAngle, double slowAngle)
	{
		_fastAngle = fastAngle;
		_slowAngle = slowAngle;
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

private:
	double _fastAngle;
	double _slowAngle;
};

}