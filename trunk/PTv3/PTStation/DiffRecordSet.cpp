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
		if(!m_oneLegFed)
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

			m_oneLegFed = true;
		}
		else // the diff dataset has been fed data of first leg already
		{
			// calculate diff value based on existing data
			int end = pOHLCRecordSet->GetEndIndex();
			int count = pOHLCRecordSet->NbElements();
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

			m_endIndex = end;
			m_nbElements = pOHLCRecordSet->NbElements();
			m_oneLegFed = false; // reset flag for new bar
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

			if(!m_oneLegFed)
			{
				DiffSeries[endIdx] = legInfo->GetLastValue();
				m_oneLegFed = true;
			}
			else
			{
				if(legInfo->Index() > 1)
				{
					// In case the second leg
					DiffSeries[endIdx] = DiffSeries[endIdx] - pOHLCRecordSet->CloseSeries[endIdx];
				}
				else
				{
					// The first leg
					DiffSeries[endIdx] = pOHLCRecordSet->CloseSeries[endIdx] - DiffSeries[endIdx];
				}

				m_endIndex = endIdx;
				m_nbElements = pOHLCRecordSet->NbElements();
				m_oneLegFed = false; // reset flag for new bar
			}
		}
		else if(endIdx == legInfo->GetEndIndex())// current bar
		{
			legInfo->SetLastValue(pOHLCRecordSet->CloseSeries[endIdx]);
			DiffRecLegInfoPtr theOtherLegInfo = GetTheOtherLeg(legInfo->Symbol());
			if(theOtherLegInfo->GetEndIndex() == endIdx)
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
