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
}