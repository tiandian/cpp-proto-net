#include "StdAfx.h"
#include "EntryDiffTrigger.h"
#include "Portfolio.h"
#include "Leg.h"

#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <boost/foreach.hpp>

#define PARAMETER_DIFF "diff"

CEntryDiffTrigger::CEntryDiffTrigger(CPortfolio* parentPortfolio):
	m_Portfolio(parentPortfolio),
	m_thresholdDiff(-1)
{
}


CEntryDiffTrigger::~CEntryDiffTrigger(void)
{
}

bool CEntryDiffTrigger::OnConditionCheck( CTP::Quote* quote )
{
	int idx = 0;
	BOOST_FOREACH(const boost::shared_ptr<CLeg>& leg, m_Portfolio->GetLegs())
	{
		const std::string& symb = quote->symbol();
		if(symb == leg->GetSymbol())
		{
			protoc::PosiDirectionType side = leg->GetSide();

			double val = 0;
			
			if(side == protoc::LONG)
			{
				val = quote->ask();

				if(leg->GetStatus() == UNOPENED)
					leg->SetOpenLimitPrice(quote->ask());
				else if(leg->GetStatus() == OPENED)
					leg->SetCloseLimitPrice(quote->bid());
			}
			else if(side == protoc::SHORT)
			{
				val = -quote->bid();

				if(leg->GetStatus() == UNOPENED)
					leg->SetOpenLimitPrice(quote->bid());
				else if(leg->GetStatus() == OPENED)
					leg->SetCloseLimitPrice(quote->ask());
			}

			if(idx < m_legValues.size())
			{
				m_legValues[idx] = val; 
			}
			else
			{
				m_legValues.push_back(val);
			}

			break;
		}
		++idx;
	}

	// compute diff
	if(m_legValues.size() == m_Portfolio->GetLegs().size())
	{
		double diff = 0;
		BOOST_FOREACH(double v, m_legValues)
		{
			diff += v;
		}

		std::cout << "Current Diff: " << diff << std::endl;

		if(diff >= m_thresholdDiff)
			return true;
	}

	return false;
}

void CEntryDiffTrigger::OnTriggered()
{
	InvokeHandler(m_Portfolio->GetID());
}

void CEntryDiffTrigger::SetParameters( TriggerParamMap& paramMap )
{
	TriggerParamMapIter it = paramMap.find(PARAMETER_DIFF);
	if(it != paramMap.end())
	{
		double val = boost::lexical_cast<double>(it->second);
		m_thresholdDiff = val;
	}
}

void CEntryDiffTrigger::GetParameters( TriggerParamMap& paramMap )
{
	std::string val = boost::str(boost::format("%f") % m_thresholdDiff);
	paramMap.insert(std::pair<std::string, std::string>(PARAMETER_DIFF, val));
}

//void CEntryDiffTrigger::BuildFactors()
//{
//	m_factors.clear();
//	BOOST_FOREACH(const boost::shared_ptr<CLeg>& leg, m_Portfolio->GetLegs())
//	{
//		protoc::PosiDirectionType side = leg->GetSide();
//		const std::string& symb = leg->GetSymbol();
//
//		if(side == protoc::LONG)
//		{
//			m_factors.insert(std::pair<std::string, int>(symb, 1));
//		}
//		else if(side == protoc::SHORT)
//		{
//			m_factors.insert(std::pair<std::string, int>(symb, -1));
//		}
//	}
//}
