#include "StdAfx.h"
#include "ScalperStrategy.h"


CScalperStrategy::CScalperStrategy(void)
{
}


CScalperStrategy::~CScalperStrategy(void)
{
}

void CScalperStrategy::ApplySettings( const std::string& settingData )
{
	entity::ScalperSettings settings;
	settings.ParseFromString(settingData);

	SetOpenPosiCond(GREATER_EQUAL_THAN, settings.threshold());

	logger.Info(boost::str(boost::format("Scalper Threshold: %f") % settings.threshold()));
}

POSI_OPER CScalperStrategy::NextOperation( POSI_OPER oper )
{
	switch (oper)
	{
	case OPEN_POSI:
		return DO_NOTHING;
	default:
		return DO_NOTHING;
	}
}

void CScalperStrategy::Test()
{
	if(!m_isRunning) return;

	double diff = Portfolio()->Difference();
	CStrategy<double>::Test(diff, OPEN_POSI);
}

void CScalperStrategy::DoOpenPostion()
{
	logger.Info(boost::str(boost::format("Portf (%s) OPEN & CLOSE position by strategy") % Portfolio()->ID().c_str()));

}
