#include "StdAfx.h"
#include "TimeNSalePacket.h"


CTimeNSalePacket::CTimeNSalePacket(void):
CMessage(TIME_AND_SALES)
{
	memset(&m_innerData, 0, sizeof(m_innerData));
}


CTimeNSalePacket::~CTimeNSalePacket(void)
{
}
