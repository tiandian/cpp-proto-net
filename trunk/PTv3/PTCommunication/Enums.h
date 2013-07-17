#pragma once
using namespace System;

namespace PTEntity {
	
	public enum class TradeDirectionType : int 
	{ 
		BUY = 48, 
		SELL = 49
	};

	public enum class ServerType : int
	{
		SERV_QUOTE = 0,
		SERV_TRADE = 1
	};

	public enum class PosiDirectionType : int
	{
		NET = 49,
		LONG = 50,
		SHORT = 51
	};

	public enum class LegStatus : int
	{
		UNOPENED = 0,
		IS_OPENING = 1,
		OPENED = 2,
		IS_CLOSING = 3,
		CLOSED = 4
	};

	public enum class StrategyType : int
	{
		ARBITRAGE = 0,
		CHANGE_POSITION = 1,
		SCALPER = 2
	};

	public enum class DirectionDepends : int
	{
		IGNORE_THIS = 0,
		ON_SMALL_SIZE = 1,
		ON_BIG_SIZE = 2,
		ON_SMALL_CHANGE = 3,
		ON_BIG_CHANGE = 4
	};

	public enum class StopLossCloseMethods : int
	{
		BASED_ON_NEXT_QUOTE = 0,
		BASED_ON_INPUT_LIMIT = 1
	};

	public enum class CompareCondition : int
	{
		GREATER_THAN = 0,
		GREATER_EQUAL_THAN = 1,
		LESS_THAN = 2,
		LESS_EQUAL_THAN = 3
	};

	public enum class PosiOffsetFlag : int
	{
		OPEN = 0,
		CLOSE = 1
	};
}