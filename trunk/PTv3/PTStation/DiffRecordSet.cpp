#include "StdAfx.h"
#include "DiffRecordSet.h"
#include "OHLCRecordSet.h"


CDiffRecordSet::CDiffRecordSet(COHLCRecordSet* pFirstLeg, COHLCRecordSet* pSecondLeg)
	: m_oneLegFed(false)
	, m_endIndex(-1)
	, m_nbElements(0)
{
	m_totalCount = pFirstLeg->GetSize();
	DiffSeries = boost::shared_array<double>(new double[m_totalCount]);
	COHLCRecordSet::ResetArray(DiffSeries.get(), m_totalCount);
	
	DiffRecLegInfoPtr firstLegInfo(new CDiffRecLegInfo(1, pFirstLeg->Symbol()));
	m_diffRecLegs.insert(make_pair(firstLegInfo->Symbol(), firstLegInfo));
	DiffRecLegInfoPtr secondLegInfo(new CDiffRecLegInfo(2, pSecondLeg->Symbol()));
	m_diffRecLegs.insert(make_pair(secondLegInfo->Symbol(), secondLegInfo));
}


CDiffRecordSet::~CDiffRecordSet(void)
{
}

int CDiffRecordSet::Calculate( COHLCRecordSet* pOHLCRecordSet )
{
	const string& symbol = pOHLCRecordSet->Symbol();
	DiffRecLegMapIter iter = m_diffRecLegs.find(symbol);
	const DiffRecLegInfoPtr& legInfo = iter->second;
	if(legInfo->NbElements() < 1)
	{
		// In case this leg has not been fed any data
		if(m_nbElements < 1)
		{
			// The diff dataset has not been fed as well
			int begin = pOHLCRecordSet->GetBeginIndex();
			int end = pOHLCRecordSet->GetEndIndex();
			for(int i = begin; i <= end; ++i)
			{
				DiffSeries[i] = pOHLCRecordSet->CloseSeries[i];
			}
			legInfo->SetEndIndex(end);
			legInfo->NbElements(pOHLCRecordSet->NbElements());
			m_endIndex = end;
			m_nbElements = pOHLCRecordSet->NbElements();
		}
		else // the diff dataset has been fed data of first leg already
		{
			// calculate diff value based on existing data
			int end = m_endIndex <= pOHLCRecordSet->GetEndIndex() ? m_endIndex : pOHLCRecordSet->GetEndIndex();
			int count = m_nbElements <= pOHLCRecordSet->NbElements() ? m_nbElements : pOHLCRecordSet->NbElements();
			int begin = end - count + 1;
			if(legInfo->Index() > 1)
			{
				// In case the second leg
				for(int i = begin; i <= end; ++i )
				{
					DiffSeries[i] = DiffSeries[i] - pOHLCRecordSet->CloseSeries[i];
				}
			}
			else
			{
				// The first leg
				for(int i = begin; i <= end; ++i )
				{
					DiffSeries[i] = pOHLCRecordSet->CloseSeries[i] - DiffSeries[i];
				}
			}
			legInfo->SetEndIndex(pOHLCRecordSet->GetEndIndex());
			legInfo->NbElements(pOHLCRecordSet->NbElements());
		}
	}
	else
	{
		int endIdx = pOHLCRecordSet->GetEndIndex();
		if(endIdx > legInfo->GetEndIndex())	// new bar
		{
			legInfo->SetEndIndex(endIdx);
			legInfo->NbElements(pOHLCRecordSet->NbElements());
			legInfo->SetLastValue(pOHLCRecordSet->CloseSeries[endIdx]);

			if(endIdx > m_endIndex)
			{
				DiffSeries[endIdx] = legInfo->GetLastValue();
				m_endIndex = endIdx;
				m_nbElements = pOHLCRecordSet->NbElements();
			}
			else
			{
				if(legInfo->Index() > 1)
				{
					// In case the second leg
					DiffSeries[m_endIndex] = DiffSeries[m_endIndex] - pOHLCRecordSet->CloseSeries[m_endIndex];
				}
				else
				{
					// The first leg
					DiffSeries[m_endIndex] = pOHLCRecordSet->CloseSeries[m_endIndex] - DiffSeries[m_endIndex];
				}
			}
		}
		else if(endIdx == legInfo->GetEndIndex())// current bar
		{
			legInfo->SetLastValue(pOHLCRecordSet->CloseSeries[endIdx]);
			DiffRecLegInfoPtr theOtherLegInfo = GetTheOtherLeg(legInfo->Symbol());
			if(theOtherLegInfo->GetBeginIndex() == endIdx)
			{
				if(legInfo->Index() > 1)
				{
					// In case the second leg
					DiffSeries[endIdx] = theOtherLegInfo->GetLastValue() - legInfo->GetLastValue();
				}
				else
				{
					// The first leg
					DiffSeries[endIdx] = legInfo->GetLastValue() - theOtherLegInfo->GetLastValue();
				}
			}
			else
			{
				DiffSeries[endIdx] = legInfo->GetLastValue();
			}
		}
	}

	return m_endIndex;
}

DiffRecLegInfoPtr CDiffRecordSet::GetTheOtherLeg( string theLegSymb )
{
	for(DiffRecLegMapIter iter = m_diffRecLegs.begin(); iter != m_diffRecLegs.end(); ++iter )
	{
		if(iter->first != theLegSymb)
			return iter->second;
	}

	return DiffRecLegInfoPtr();
}
