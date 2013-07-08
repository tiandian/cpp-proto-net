#pragma once

#include "Enums.h"
#include "entity/message.pb.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;

namespace PTEntity {

public ref class LegUpdateItem
{
public:
	LegUpdateItem(const entity::LegUpdateItem* pEntity);

	property String^ Symbol
	{
		String^ get()
		{
			return _symbol; 
		}
	}

	property LegStatus Status
	{
		LegStatus get()
		{
			return _status; 
		}
	}

	property double Last
	{
		double get()
		{
			return _last; 
		}
	}

	property double Ask
	{
		double get()
		{
			return _ask; 
		}
	}

	property int AskSize
	{
		int get()
		{
			return _askSize; 
		}
	}

	property double Bid
	{
		double get()
		{
			return _bidSize; 
		}
		void set(double val)
		{
			_bidSize = val;
		}
	}

	property int BidSize
	{
		int get()
		{
			return _bidSize; 
		}
	}

private:
	String ^_symbol;
	LegStatus _status;
	double _last;
	double _ask;
	int _askSize;
	double _bid;
	int _bidSize;
};

}

