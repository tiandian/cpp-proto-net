#pragma once

#include "entity/message.pb.h"

#include <string>

using namespace std;

class CSymbolInfo
{
public:
	CSymbolInfo(const string& symbol, const string& exchId, const string& prodId);
	~CSymbolInfo(void);

	const string& Instrument() const { return m_innerItem.instrument(); }
	void Instrument(const string& val) { m_innerItem.set_instrument(val); }

	const std::string& ExchangeID() const { return m_innerItem.exchangeid(); }
	void ExchangeID(const std::string& val) { m_innerItem.set_exchangeid(val); }

	const std::string& ExchangeInstID() const { return m_innerItem.exchangeinstid(); }
	void ExchangeInstID(const std::string& val) { m_innerItem.set_exchangeinstid(val); }

	const std::string& ProductID() const { return m_innerItem.productid(); }
	void ProductID(const std::string& val) { m_innerItem.set_productid(val); }

	int VolumeMultiple() const { return m_innerItem.volumemultiple(); }
	void VolumeMultiple(int val) { m_innerItem.set_volumemultiple(val); }

	double PriceTick() const { return m_innerItem.pricetick(); }
	void PriceTick(double val) { m_innerItem.set_pricetick(val); }

	bool Ready() const { return m_ready; }
	void Ready(bool val) { m_ready = val; }

	entity::SymbolInfo& InnerItem(){ return m_innerItem; }

private:

	entity::SymbolInfo m_innerItem;

	bool m_ready;
	
};

