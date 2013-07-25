#include "StdAfx.h"
#include "SettingChangeTrace.h"
#include "globalmembers.h"
#include "DoubleCompare.h"

void PrintIntegerChange(const char* desc, int oldVal, int newVal)
{
	if(oldVal != newVal)
	{
		logger.Info(boost::str(boost::format("%s has been changed : %d => %d")
		% desc % oldVal %newVal));
	}
}

void PrintDoubleChange(const char* desc, double oldVal, double newVal)
{
	if(!(DoubleEqual(oldVal, newVal)))
	{
		logger.Info(boost::str(boost::format("%s has been changed : %.2f => %.2f")
		% desc % oldVal %newVal));
	}
}
