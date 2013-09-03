#include "StdAfx.h"
#include "HistDataReader.h"
#include "TechStrategyDefs.h"
#include "PriceBarGen.h"
#include "OHLCRecordSet.h"
#include "globalmembers.h"

#include <boost/filesystem.hpp>
using namespace boost::filesystem;

CHistDataReader::CHistDataReader(const string& symbol, unsigned int precision)
	: m_symbol(symbol)
	, m_precision(precision)
{
}


CHistDataReader::~CHistDataReader(void)
{
}

void CHistDataReader::Read( COHLCRecordSet* pRecordSet, CPriceBarGen* pPriceBarGen )
{
	assert(pRecordSet != NULL);

	string filePath = boost::str(boost::format("%s/%s-%u.txt") % HIST_DATA_FOLDER % m_symbol % m_precision);
	path p(filePath);
	if(exists(p))
	{
		ifstream f(filePath);
		if(f.good())
		{
			while(!f.eof())
			{
				f.getline(m_buf, HIST_LINE_MAX_LENGTH);

				string timestamp;
				double open, high, low, close;
				bool succ = ParseLine(&timestamp, &open, &high, &low, &close);
				if(succ)
				{
					unsigned int idx = pPriceBarGen->GetIndex(timestamp);
					pRecordSet->Set(idx, open, high, low, close);
				}
			}
		}
		else
		{
			logger.Error(boost::str(boost::format("Failed to open file %s") % filePath));
		}
	}
	
}

bool CHistDataReader::ParseLine( string* timestamp, double* open, double* high, double* low, double* close )
{
	char timeBuf[10];
	int got = sscanf(m_buf, "%s %lf, %lf, %lf, %lf", timeBuf, open, high, low, close);
	*timestamp = timeBuf;
	return got == 5;
}
