#pragma once

#include "ThostFtdcMdApi.h"

#include <string>
#include <boost/thread.hpp>

class CConnectionTester : public CThostFtdcMdSpi
{
public:
	CConnectionTester(void);
	~CConnectionTester(void);

	///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
	virtual void OnFrontConnected();

	///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
	///@param nReason 错误原因
	///        0x1001 网络读失败
	///        0x1002 网络写失败
	///        0x2001 接收心跳超时
	///        0x2002 发送心跳失败
	///        0x2003 收到错误报文
	virtual void OnFrontDisconnected(int nReason);

	bool Validate(const std::string& address, int* delay);

private:

	void ConnectMarketFunc(char* pAddress);

	CThostFtdcMdApi* m_pUserApi;
	boost::thread m_thConnect;
	boost::condition_variable m_condConnectDone;
	boost::mutex m_mutex;
};

