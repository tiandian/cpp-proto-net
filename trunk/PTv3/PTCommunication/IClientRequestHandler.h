#pragma once

#include "ServerLoginReturn.h"
#include "PortfolioUpdateItem.h"

using namespace System;
using namespace PTEntity;

namespace PTCommunication {

public interface class IClientRequestHandler
{
	void OnLoginReturned(LoginReturn ^loginReturn);
	void OnServerLoginReturned(ServerLoginReturn ^loginReturn);
	void OnPortfolioUpdate(PortfolioUpdateItem ^portfolioUpdateItem);
};

}