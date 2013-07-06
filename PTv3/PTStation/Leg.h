#pragma once

#include "entity/message.pb.h"

class CLeg
{
public:
	CLeg(int legId, const entity::LegItem& srcLegItem);
	~CLeg(void);

	int Id(){ return m_legId; }
	const string& Symbol(){ return m_symbol; }

	entity::PosiDirectionType Side(){ return m_side; }
	double Last(){ return m_last; }
	double Ask(){ return m_ask; }
	int AskSize() { return m_askSize; }
	double Bid(){ return m_bid; }
	int BidSize() { return m_bidSize; }

	void UpdateLast(double last){ m_last = last; }
	void UpdateAsk(double ask){ m_ask = ask; }
	void UpdateAskSize(int askSize){ m_askSize = askSize; }
	void UpdateBid(double bid){ m_bid = m_bid; }
	void UpdateBidSize(int bidSize){ m_bidSize = m_bidSize; }
	void UpdateTimestamp(){}

	const entity::LegUpdateItem& GetUpdated();

private:
	entity::LegUpdateItem m_legUpdate;

	int m_legId;
	string m_symbol;
	int m_ratio;
	entity::PosiDirectionType m_side;
	bool m_isPreferred;

	entity::LegStatus m_status;
	double m_last;
	double m_ask;
	int m_askSize;
	double m_bid;
	int m_bidSize;

};

typedef boost::shared_ptr<CLeg> LegPtr;

