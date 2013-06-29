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
}