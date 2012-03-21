#pragma once

#include "protobuf_gen/trade.pb.h"

#include <string>

class CPortfolio;

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

private:
	CPortfolio* m_pPortfolio;

	std::string m_symbol;
	protoc::PosiDirectionType m_side;
	int m_ratio;
	int m_quantity;

	bool m_isOpening;
	protoc::OrderStatusType m_orderStatus;
};

