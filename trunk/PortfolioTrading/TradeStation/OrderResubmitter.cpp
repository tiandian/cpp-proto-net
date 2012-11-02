#include "StdAfx.h"
#include "OrderResubmitter.h"


COrderResubmitter::COrderResubmitter(trade::InputOrder* pInputOrd):
m_pendingOrder(pInputOrd)
{
}


COrderResubmitter::~COrderResubmitter(void)
{
}
