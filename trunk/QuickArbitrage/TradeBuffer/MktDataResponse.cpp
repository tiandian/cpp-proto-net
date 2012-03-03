#include "StdAfx.h"
#include "MktDataResponse.h"


CMktDataResponse::CMktDataResponse(void)
{
}


CMktDataResponse::~CMktDataResponse(void)
{
}

void CMktDataResponse::OnFrontConnected()
{

}

void CMktDataResponse::OnFrontDisconnected( int nReason )
{

}

void CMktDataResponse::OnHeartBeatWarning( int nTimeLapse )
{

}

void CMktDataResponse::OnRspUserLogin( CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{

}

void CMktDataResponse::OnRspUserLogout( CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{

}

void CMktDataResponse::OnRspError( CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{

}

void CMktDataResponse::OnRspSubMarketData( CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{

}

void CMktDataResponse::OnRspUnSubMarketData( CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast )
{

}

void CMktDataResponse::OnRtnDepthMarketData( CThostFtdcDepthMarketDataField *pDepthMarketData )
{

}
