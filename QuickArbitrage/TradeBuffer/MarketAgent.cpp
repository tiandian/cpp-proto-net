#include "StdAfx.h"
#include "MarketAgent.h"

// 配置参数
char FRONT_ADDR[] = "tcp://asp-sim2-md1.financial-trading-platform.com:26213";		// 前置地址
TThostFtdcBrokerIDType	BROKER_ID = "2030";				// 经纪公司代码
TThostFtdcInvestorIDType INVESTOR_ID = "00092";			// 投资者代码
TThostFtdcPasswordType  PASSWORD = "888888";			// 用户密码
char *ppInstrumentID[] = {"cu1010", "cu1009"};			// 行情订阅列表
int iInstrumentID = 2;									// 行情订阅数量

// 请求编号
int iRequestID = 0;


CMarketAgent::CMarketAgent(void):
	m_pUserApi(NULL)
{
}

CMarketAgent::~CMarketAgent(void)
{
	if(m_pUserApi != NULL)
		m_pUserApi->Release();
}

void CMarketAgent::Startup()
{
	// 初始化UserApi
	m_pUserApi = CThostFtdcMdApi::CreateFtdcMdApi();			// 创建UserApi
	//CThostFtdcMdSpi* pUserSpi = new CMdSpi();
	m_pUserApi->RegisterSpi(&m_mktResponse);						// 注册事件类
	m_pUserApi->RegisterFront(FRONT_ADDR);					// connect
	m_pUserApi->Init();
}
