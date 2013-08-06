#pragma once

class COHLCRecordSet;

class CHistDataReader
{
public:
	CHistDataReader(void);
	~CHistDataReader(void);

	void Read(COHLCRecordSet* pRecordSet);
};

