#pragma once

#include <PushFrameworkInc.h>

using namespace PushFramework;

class CTradingServer : public Server
{
public:
	CTradingServer(void);
	~CTradingServer(void);

	void CreateServices();
};

