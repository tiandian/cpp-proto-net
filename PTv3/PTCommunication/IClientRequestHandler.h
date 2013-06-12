#pragma once

#include "ServerLoginReturn.h"

using namespace System;
using namespace PTEntity;

namespace PTCommunication {

public interface class IClientRequestHandler
{
	void OnLoginReturned(LoginReturn ^loginReturn);
	void OnServerLoginReturned(ServerLoginReturn ^loginReturn);
};

}