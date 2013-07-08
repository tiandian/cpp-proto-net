#pragma once

#include "Enums.h"

using namespace System;

namespace PTEntity {

public ref class StrategyUpdateItem
{
public:
	StrategyUpdateItem(void){}

	property StrategyType Kind
	{
		virtual StrategyType get() abstract;
	}
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

}