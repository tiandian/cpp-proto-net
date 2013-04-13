#include "InputOrder.h"

#ifndef WIN32
#define strcpy_s strcpy
#define _strdup strdup
#endif

CInputOrder::CInputOrder(void)
{
}


CInputOrder::~CInputOrder(void)
{
}

void CInputOrder::set_brokerid( const string& brokerId )
{
	strcpy_s(m_innerOrderItem.BrokerID, brokerId.c_str());
}

void CInputOrder::set_investorid( const string& investorId )
{
	strcpy_s(m_innerOrderItem.BrokerID, investorId.c_str());
}

void CInputOrder::set_symbol( const string& symbol )
{
	m_symbol = symbol;
	strcpy_s(m_innerOrderItem.InstrumentID, symbol.c_str());
}

void CInputOrder::set_orderref( const string& orderRef )
{
	m_orderRef = orderRef;
	strcpy_s(m_innerOrderItem.OrderRef, orderRef.c_str());
}

void CInputOrder::set_orderpricetype( trade::OrderPriceTypeType ordPriceType )
{
	m_innerOrderItem.OrderPriceType = ordPriceType;
}

void CInputOrder::set_direction( trade::TradeDirectionType tradeDirection )
{
	m_direction = tradeDirection;
	m_innerOrderItem.Direction = tradeDirection;
}

void CInputOrder::set_combhedgeflag( const string& hedgeFlag )
{
	m_innerOrderItem.CombHedgeFlag[0] = hedgeFlag[0];
}

void CInputOrder::set_comboffsetflag( const string& offsetFlag )
{
	m_offsetFlag = offsetFlag;
	m_innerOrderItem.CombOffsetFlag[0] = offsetFlag[0];
}

void CInputOrder::set_limitprice( double lmtPx )
{
	m_innerOrderItem.LimitPrice = lmtPx;
}

void CInputOrder::set_volumetotaloriginal( int volTotal )
{
	m_innerOrderItem.VolumeTotalOriginal = volTotal;
}

void CInputOrder::set_timecondition( trade::TimeConditionType timeConditionType )
{
	m_innerOrderItem.TimeCondition = timeConditionType;
}

void CInputOrder::set_volumecondition( trade::VolumeConditionType volConditionType )
{
	m_innerOrderItem.VolumeCondition = volConditionType;
}

void CInputOrder::set_minvolume( int minVol )
{
	m_innerOrderItem.MinVolume = minVol;
}

void CInputOrder::set_contingentcondition( trade::ContingentConditionType contingentCond )
{
	m_innerOrderItem.ContingentCondition = contingentCond;
}

void CInputOrder::set_forceclosereason( trade::ForceCloseReasonType forceCloseReason )
{
	m_innerOrderItem.ForceCloseReason = forceCloseReason;
}

void CInputOrder::set_isautosuspend( bool isAutoSuspend )
{
	m_innerOrderItem.IsAutoSuspend = isAutoSuspend;
}

void CInputOrder::set_requestid( int reqId )
{
	m_innerOrderItem.RequestID = reqId;
}

void CInputOrder::set_userforceclose( bool userForceClose )
{
	m_innerOrderItem.UserForceClose = userForceClose;
}
