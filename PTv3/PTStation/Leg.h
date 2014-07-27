#pragma once

#include "entity/message.pb.h"
#include <limits>

template<class T>
T SafeSetValue(T val)
{
	return (val == numeric_limits<T>::max() || val == numeric_limits<T>::min()) ? ((T)0) : val;
}

class CLeg
{
public:
	CLeg(int legId, const entity::LegItem& srcLegItem, entity::LegUpdateItem* pUpdateItem);
	~CLeg(void);

	int Id(){ return m_legId; }
	const string& Symbol(){ return m_symbol; }

	entity::PosiDirectionType Side(){ return m_side; }
	double Last(){ return m_last; }
	double Ask(){ return m_ask; }
	int AskSize() { return m_askSize; }
	double Bid(){ return m_bid; }
	int BidSize() { return m_bidSize; }
	int Ratio() { return m_ratio; }
	bool IsPreferred() { return m_isPreferred; }
	double MinPriceChange() { return m_minPriceChange; }

	void UpdateLast(double last){ m_last = SafeSetValue(last); }
	void UpdateAsk(double ask){ m_ask = SafeSetValue(ask); }
	void UpdateAskSize(int askSize){ m_askSize = SafeSetValue(askSize); }
	void UpdateBid(double bid){ m_bid = SafeSetValue(bid); }
	void UpdateBidSize(int bidSize){ m_bidSize = SafeSetValue(bidSize); }
	void UpdateTimestamp(){}

	void GetUpdated();

private:
	entity::LegUpdateItem* m_legUpdate;

	int m_legId;
	string m_symbol;
	int m_ratio;
	entity::PosiDirectionType m_side;
	bool m_isPreferred;
	double m_minPriceChange;

	entity::LegStatus m_status;
	double m_last;
	double m_ask;
	int m_askSize;
	double m_bid;
	int m_bidSize;

};

typedef boost::shared_ptr<CLeg> LegPtr;

