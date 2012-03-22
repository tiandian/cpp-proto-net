#pragma once

#include "protobuf_gen/trade.pb.h"

#include <string>

class CPortfolio;

typedef enum _Leg_Status
{
	UNOPENED,
	IS_OPENING,
	OPENED,
	IS_CLOSING,
	CLOSED = UNOPENED 
} LEG_STATUS;

class CLeg
{
public:
	CLeg(CPortfolio* portfolio);
	~CLeg(void);

	const std::string& GetSymbol() {return m_symbol;}
	void SetSymbol(const std::string& symbol){ m_symbol = symbol;}

	protoc::PosiDirectionType GetSide(){return m_side;}
	void SetSide(protoc::PosiDirectionType side){m_side = side;}

	int GetRatio() {return m_ratio;}
	void SetRatio(int ratio) {m_ratio = ratio;}

	int GetQuantity();

	bool GetIsOpened(){ return m_legStatus == OPENED; }
	
	LEG_STATUS GetStatus() { return m_legStatus; }
	void SetStatus(LEG_STATUS status) { m_legStatus = status; }

	protoc::OrderStatusType GetOrderStatus(){ return m_orderStatus; }
	void SetOrderStatus(protoc::OrderStatusType status){ m_orderStatus = status; }

	protoc::OrderSubmitStatusType GetOrderSubmitStatus(){ return m_orderSubmitStatus; }
	void SetOrderSubmitStatus(protoc::OrderSubmitStatusType status){ m_orderSubmitStatus = status; }

	const std::string& GetOrderRef() { return m_orderRef; }
	void SetOrderRef(const std::string& orderRef) { m_orderRef = orderRef; }

	const std::string& GetMessage() { return m_sMsg; }
	void SetMessage(const std::string& msg) { m_sMsg = msg; }

private:
	CPortfolio* m_pPortfolio;

	std::string m_symbol;
	protoc::PosiDirectionType m_side;
	int m_ratio;
	
	LEG_STATUS m_legStatus;
	protoc::OrderStatusType m_orderStatus;
	protoc::OrderSubmitStatusType m_orderSubmitStatus;

	std::string m_orderRef;
	
	std::string m_sMsg;
};

