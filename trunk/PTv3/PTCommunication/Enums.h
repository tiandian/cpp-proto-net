#pragma once
using namespace System;

namespace PTCommunication {

	public enum class ServerType { Quote, Trade };
}

namespace PTEntity {
	
	public enum class TradeDirectionType : int 
	{ 
		BUY = 48, 
		SELL = 49
	};

}