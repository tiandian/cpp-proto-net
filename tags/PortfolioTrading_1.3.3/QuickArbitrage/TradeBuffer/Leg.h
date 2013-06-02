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
	const char* GetSideText();
	void SetSide(protoc::PosiDirectionType side){m_side = side;}

	int GetRatio() {return m_ratio;}
	void SetRatio(int ratio) {m_ratio = ratio;}

	int GetQuantity();

	bool GetIsOpened(){ return m_legStatus == OPENED; }
	
	LEG_STATUS GetStatus() { return m_legStatus; }
	const char* GetStatusText();
	void SetStatus(LEG_STATUS status) { m_legStatus = status; }

	protoc::OrderStatusType GetOrderStatus(){ return m_orderStatus; }
	const char* GetOrderStatusText();
	void SetOrderStatus(protoc::OrderStatusType status){ m_orderStatus = status; }

	protoc::OrderSubmitStatusType GetOrderSubmitStatus(){ return m_orderSubmitStatus; }
	const char* GetOrderSubmitStatusText();
	void SetOrderSubmitStatus(protoc::OrderSubmitStatusType status){ m_orderSubmitStatus = status; }

	const std::string& GetOrderRef() { return m_orderRef; }
	void SetOrderRef(const std::string& orderRef) { m_orderRef = orderRef; }

	const std::string& GetStatusMessage() { return m_sMsg; }
	void SetMessage(const std::string& msg) { m_sMsg = msg; }

	protoc::OrderPriceTypeType GetOpenOrderPriceType() { return m_openOrderPriceType; }
	void SetOpenOrderPriceType(protoc::OrderPriceTypeType priceType) { m_openOrderPriceType = priceType; }

	double GetOpenLimitPrice() { return m_openLimitPrice; }
	void SetOpenLimitPrice(double price) { m_openLimitPrice = price; }

	protoc::OrderPriceTypeType GetCloseOrderPriceType() { return m_closeOrderPriceType; }
	void SetCloseOrderPriceType(protoc::OrderPriceTypeType priceType) { m_closeOrderPriceType = priceType; }

	double GetCloseLimitPrice() { return m_closeLimitPrice; }
	void SetCloseLimitPrice(double price) { m_closeLimitPrice = price; }

	double GetCost() { return m_cost; }
	void SetCost(double cost){ m_cost = cost; }



private:
	CPortfolio* m_pPortfolio;

	std::string m_symbol;
	protoc::PosiDirectionType m_side;
	int m_ratio;
	
	LEG_STATUS m_legStatus;
	protoc::OrderStatusType m_orderStatus;
	protoc::OrderSubmitStatusType m_orderSubmitStatus;

	protoc::OrderPriceTypeType m_openOrderPriceType;
	double m_openLimitPrice;
	double m_cost;
	protoc::OrderPriceTypeType m_closeOrderPriceType;
	double m_closeLimitPrice;

	std::string m_orderRef;
	
	std::string m_sMsg;
};

