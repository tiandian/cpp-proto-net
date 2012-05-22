#include "StdAfx.h"
#include "PositionDetailMsg.h"


CPositionDetailMsg::CPositionDetailMsg(void):
CMessage(POSITION_DETAIL)
{
	memset(&m_innerData, 0, sizeof(m_innerData));
}


CPositionDetailMsg::~CPositionDetailMsg(void)
{
}
