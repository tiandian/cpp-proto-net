#pragma once

#include <boost/shared_array.hpp>

class COHLCRecordSet;

class CDiffRecLegInfo
{
public:
	CDiffRecLegInfo(int idx, string symbol) 
		: m_symbol(symbol), m_endIndex(-1), m_nbElements(0), m_index(idx) {}
	int GetEndIndex(){ return m_endIndex; }
	void SetEndIndex(int val) { m_endIndex = val; }
	int GetBeginIndex() { return m_endIndex - m_nbElements + 1; }
	int NbElements(){ return m_nbElements; }
	void NbElements(int val){ m_nbElements = val; }
	
	const string& Symbol() { return m_symbol; }
	int Index() { return m_index; }

	double GetLastValue(){ return m_lastValue; }
	void SetLastValue(double val){ m_lastValue = val; }

private:

	string m_symbol;
	int m_endIndex;
	int m_nbElements;
	int m_index;
	double m_lastValue;
};

typedef boost::shared_ptr<CDiffRecLegInfo> DiffRecLegInfoPtr;
typedef std::map<string, DiffRecLegInfoPtr> DiffRecLegMap;
typedef DiffRecLegMap::iterator DiffRecLegMapIter;

class CDiffRecordSet
{
public:
	CDiffRecordSet(COHLCRecordSet* pFirstLeg, COHLCRecordSet* pSecondLeg);
	~CDiffRecordSet(void);

	boost::shared_array<double> DiffSeries;

	int GetSize(){ return m_totalCount; }
	int GetEndIndex(){ return m_endIndex; }
	int GetBeginIndex() { return m_endIndex - m_nbElements + 1; }
	int NbElements(){ return m_nbElements; }

	int Calculate(COHLCRecordSet* pOHLCRecordSet);

private:
	DiffRecLegInfoPtr GetTheOtherLeg(string theLegSymb);

	DiffRecLegMap m_diffRecLegs;

	int m_totalCount;
	int m_endIndex;
	int m_nbElements;

	bool m_oneLegFed;
};

typedef boost::shared_ptr<CDiffRecordSet> DiffRecordSetPtr;