#pragma once

#include "Enums.h"
#include "StrategyItem.h"
#include "entity/message.pb.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;

namespace PTEntity {

public ref class LegItem
{
public: 
	LegItem(){}

	property int LegId
	{
		int get()
		{
			return _legId;
		}
		void set(int val)
		{
			_legId = val;
		}
	}

	property String^ Symbol
	{
		String^ get()
		{
			return _symbol;
		}
		void set(String ^val)
		{
			_symbol = val;
		}
	}

	property int Ratio
	{
		int get()
		{
			return _ratio;
		}
		void set(int val)
		{
			_ratio = val;
		}
	}

	property PosiDirectionType Side	
	{
		PosiDirectionType get()
		{
			return _side;
		}
		void set(PosiDirectionType val)
		{
			_side = val;
		}
	}

	property bool IsPreferred
	{
		bool get()
		{
			return _isPreferred;
		}
		void set(bool val)
		{
			_isPreferred = val;
		}
	}

	void To(entity::LegItem* pNativeLegItem);

private:

	int _legId;
	String ^_symbol;
	int _ratio;
	PosiDirectionType _side;
	bool _isPreferred;
};

public ref class PortfolioItem
{
public:
	PortfolioItem(void)
	{
		_legs = gcnew List<LegItem^>();;
	}

	property String^ ID
	{
		String^ get()
		{
			return _id;
		}
		void set(String^ val)
		{
			_id = val;
		}
	}

	property int Quantity
	{
		int get()
		{
			return _quantity;
		}
		void set(int val)
		{
			_quantity = val;
		}
	}

	property int MaxCancel
	{
		int get()
		{
			return _maxCancel;
		}
		void set(int val)
		{
			_maxCancel = val;
		}
	}

	property int MaxTriggerTimes
	{
		int get()
		{
			return _maxTriggerTimes;
		}
		void set(int val)
		{
			_maxTriggerTimes = val;
		}
	}

	property List<LegItem^>^ Legs
	{
		List<LegItem^>^ get()
		{
			return _legs;
		}
	}

	property StrategyItem^ Strategy
	{
		StrategyItem^ get()
		{
			return _strategy;
		}
		void set(StrategyItem ^val)
		{
			_strategy = val;
		}
	}

	void To(entity::PortfolioItem* pNativePortfolio);

private:
	String ^_id;
	int _quantity;
	List<LegItem^>^ _legs;
	int _maxCancel;
	int _maxTriggerTimes;
	StrategyItem ^_strategy;
};

}
