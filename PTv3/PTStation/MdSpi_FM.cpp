#include "StdAfx.h"
#include "MdSpi_FM.h"
#include "QS_Configuration.h"
#include "QuoteProxy_FM.h"

#include <boost/interprocess/shared_memory_object.hpp>  
#include <boost/interprocess/mapped_region.hpp>  
#include <boost/interprocess/sync/scoped_lock.hpp> 

#ifndef WIN32
#define strcpy_s strcpy
#define _strdup strdup
#endif

using namespace boost::interprocess;

extern CQSConfiguration qsConfig;

CMdSpi::CMdSpi(CQuoteProxy* pQuoteProxy)
	: m_pQuoteProxy(pQuoteProxy)
	, m_loginWaiter(pQuoteProxy)
	, m_iRequestId(0)
{
	// Wait Login success for 10 seconds 
	m_loginWaiter.BeginWait(10);
}

CMdSpi::~CMdSpi(void)
{
	cout << "CMdSpi destructing..." << endl;
	m_pQuoteProxy = NULL;
}

void CMdSpi::OnRspError(CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> "<< "OnRspError" << endl;
	IsErrorRspInfo(pRspInfo);
}

void CMdSpi::OnFrontDisconnected(int nReason)
{
	cerr << "--->>> " << "OnFrontDisconnected" << endl;
	cerr << "--->>> Reason = " << nReason << endl;
}

void CMdSpi::OnHeartBeatWarning(int nTimeLapse)
{
	cout << "--->>> " << "OnHeartBeatWarning" << endl;
	cout << "--->>> nTimerLapse = " << nTimeLapse << endl;
}

void CMdSpi::OnFrontConnected()
{
	cout << "--->>> " << "OnFrontConnected" << endl;
	///用户登录请求
	ReqUserLogin();
}

void CMdSpi::ReqUserLogin()
{
	CUstpFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy_s(req.TradingDay, m_pQuoteProxy->MdApi()->GetTradingDay());
	strcpy_s(req.BrokerID, qsConfig.BrokerId().c_str());
	strcpy_s(req.UserID, qsConfig.Username().c_str());
	strcpy_s(req.Password, qsConfig.Password().c_str());
	strcpy_s(req.UserProductInfo, "QuoteStation for Femas");
	int iResult = m_pQuoteProxy->MdApi()->ReqUserLogin(&req, ++m_iRequestId);
	cout << "--->>> 发送用户登录请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
}

void CMdSpi::OnRspUserLogin(CUstpFtdcRspUserLoginField *pRspUserLogin, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout << "--->>> " << "OnRspUserLogin" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		m_loginWaiter.Cancel();

		///获取当前交易日
		cout << "--->>> 获取当前交易日 = " << m_pQuoteProxy->MdApi()->GetTradingDay() << endl;
		m_pQuoteProxy->GetReady();
	}
}

void CMdSpi::OnRspSubMarketData(CUstpFtdcSpecificInstrumentField *pSpecificInstrument, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout << "OnRspSubMarketData - " << (pSpecificInstrument != NULL ? pSpecificInstrument->InstrumentID : "(null)") << endl;
}

void CMdSpi::OnRspUnSubMarketData(CUstpFtdcSpecificInstrumentField *pSpecificInstrument, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout << "OnRspUnSubMarketData - " << (pSpecificInstrument != NULL ? pSpecificInstrument->InstrumentID : "(null)") << endl;
}

void CMdSpi::OnRtnDepthMarketData(CUstpFtdcDepthMarketDataField *pDepthMarketData)
{
	cout << "CMdSpi::OnRtnDepthMarketData : " << pDepthMarketData->InstrumentID << ", "
		<< pDepthMarketData->LastPrice << ", "
		<< pDepthMarketData->UpdateTime << ", "
		<< pDepthMarketData->UpdateMillisec << endl;
	m_pQuoteProxy->OnQuoteReceived(pDepthMarketData);
}

bool CMdSpi::IsErrorRspInfo(CUstpFtdcRspInfoField *pRspInfo)
{
	// 如果ErrorID != 0, 说明收到了错误的响应
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
		cerr << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
	return bResult;
}

void CMdSpi::OnRspUserLogout(CUstpFtdcRspUserLogoutField *pRspUserLogout, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{

}
