#pragma once

#include "OHLCRecordSet.h"

class CPriceBarDataSource
{
public:
	CPriceBarDataSource(void);
	~CPriceBarDataSource(void);

	void Init(int precision);
	COHLCRecordSet* RecordSet(){ return m_recordSet.get(); }

private:
	OHLCRecordSetPtr m_recordSet;
};

