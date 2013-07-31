#pragma once

#include "ThostTraderApi/ThostFtdcTraderApi.h"
#include "entity/message.pb.h"

class CRtnOrderWrapper
{
public:
	CRtnOrderWrapper(CThostFtdcOrderField* pOrder);
	~CRtnOrderWrapper(void);

	void ToEntity(trade::Order* pOrd);
	const char* OrderRef(){ return m_orderField.OrderRef; }
	trade::OrderSubmitStatusType OrderSubmitStatus()
	{ 
		return static_cast<trade::OrderSubmitStatusType>(m_orderField.OrderSubmitStatus); 
	}
	trade::OrderStatusType OrderStatus()
	{
		return static_cast<trade::OrderStatusType>(m_orderField.OrderStatus);
	}
	const char* OrderSysId() { return m_orderField.OrderSysID; }
	const char* ExchangeId() { return m_orderField.ExchangeID; }
	const char* UserId() { return m_orderField.UserID; }
	const char* StatusMsg() { return m_orderField.StatusMsg; }
	int VolumeTotal() { return m_orderField.VolumeTotal; }
	int VolumeTraded() { return m_orderField.VolumeTraded; }
	int VolumeTotalOriginal() {return m_orderField.VolumeTotalOriginal; }
	const char* Symbol() { return m_orderField.InstrumentID; }
	
	void SetVolumeTotalOriginal(int vol){ m_orderField.VolumeTotalOriginal = vol; }
	void SetVolumeTraded(int vol){ m_orderField.VolumeTraded = vol; }
	void SetOrderSubmitStatus(trade::OrderSubmitStatusType submitStatus)
	{ 
		m_orderField.OrderSubmitStatus = static_cast<TThostFtdcOrderSubmitStatusType>(submitStatus);
	}
	void SetOrderStatus(trade::OrderStatusType ordStatus)
	{
		m_orderField.OrderStatus = static_cast<TThostFtdcOrderStatusType>(ordStatus);
	}
	void ClearStatusMsg(){ memset(&m_orderField.StatusMsg, 0, sizeof(TThostFtdcErrorMsgType)); }

private:
	CThostFtdcOrderField m_orderField;
};

typedef boost::shared_ptr<CRtnOrderWrapper> RtnOrderWrapperPtr;
