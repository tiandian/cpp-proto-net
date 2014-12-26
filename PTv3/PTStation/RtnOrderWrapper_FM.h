#pragma once

#include "FemasAPI/USTPFtdcTraderApi.h"
#include "entity/message.pb.h"
#include <boost/chrono.hpp>

class CRtnOrderWrapper
{
public:
	CRtnOrderWrapper(CUstpFtdcOrderField* pOrder);
	CRtnOrderWrapper();
	~CRtnOrderWrapper(void);

	void ToEntity(trade::Order* pOrd);
	const char* OrderRef(){ return m_orderField.UserOrderLocalID; }
	trade::OrderSubmitStatusType OrderSubmitStatus()
	{
		trade::OrderStatusType os = OrderStatus();
		// always return accepted as long as order status is not ACCEPTED_NO_REPLY due to no submit status any more in Femas
		return os == trade::ACCEPTED_NO_REPLY ? trade::INSERT_SUBMITTED : trade::ACCEPTED;
	}
	trade::OrderStatusType OrderStatus()
	{
		return static_cast<trade::OrderStatusType>(m_orderField.OrderStatus);
	}
	const char* OrderSysId() { return m_orderField.OrderSysID; }
	const char* ExchangeId() { return m_orderField.ExchangeID; }
	const char* UserId() { return m_orderField.UserID; }
	const char* StatusMsg() { return ""; }	// no status msg any longer
	int VolumeTotal() { return m_orderField.VolumeRemain; }
	int VolumeTraded() { return m_orderField.VolumeTraded; }
	int VolumeTotalOriginal() {return m_orderField.Volume; }
	const char* Symbol() { return m_orderField.InstrumentID; }
	double Price() { return m_orderField.LimitPrice; }
	trade::TradeDirectionType Direction() { return static_cast<trade::TradeDirectionType>(m_orderField.Direction); }
	trade::OffsetFlagType Offset();

	void SetVolumeTotalOriginal(int vol){ m_orderField.Volume = vol; }
	void SetVolumeTraded(int vol){ m_orderField.VolumeTraded = vol; }
	void SetOrderSubmitStatus(trade::OrderSubmitStatusType submitStatus)
	{ 
	}
	void SetOrderStatus(trade::OrderStatusType ordStatus)
	{
		m_orderField.OrderStatus = static_cast<TUstpFtdcOrderStatusType>(ordStatus);
	}
	void ClearStatusMsg(){ /*memset(&m_orderField.StatusMsg, 0, sizeof(TThostFtdcErrorMsgType));*/ }

	boost::chrono::steady_clock::time_point& Timestamp(){ return m_orderTimestamp; }

	static CRtnOrderWrapper* MakeFakeSubmitOrder(CUstpFtdcInputOrderField& inputOrder);

private:
	CUstpFtdcOrderField m_orderField;
	boost::chrono::steady_clock::time_point m_orderTimestamp;
};

typedef boost::shared_ptr<CRtnOrderWrapper> RtnOrderWrapperPtr;
