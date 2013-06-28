#pragma once

#include "entity/message.pb.h"

class CLeg
{
public:
	CLeg(int legId, const entity::LegItem& srcLegItem);
	~CLeg(void);

	int Id(){ return m_legId; }
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

