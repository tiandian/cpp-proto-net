#pragma once

#include "ThostFtdcMdApi.h"
#include "MktDataResponse.h"

class CMarketAgent
{
public:
	CMarketAgent(void);
	~CMarketAgent(void);

	void Startup();

private:
	CThostFtdcMdApi* m_pUserApi;
	CMktDataResponse m_mktResponse;
};

