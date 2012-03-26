#include "StdAfx.h"
#include "OrderRepository.h"
#include "LogManager.h"

extern CLogManager logger;

//////////////////////////////////////////////////////////////////////////
// Here is COrderItem implementation
const std::string& COrderItem::GetOrderRef()
{
	return m_pLeg->GetOrderRef();
}

protoc::Order* COrderItem::GetOrder()
{
	return m_order.get();
}

void COrderItem::SetOrder( OrderPtr& pOrder )
{
	protoc::OrderSubmitStatusType submitStatus = pOrder->ordersubmitstatus();
	m_pLeg->SetOrderSubmitStatus(submitStatus);
	protoc::OrderStatusType orderStatus = pOrder->orderstatus();
	m_pLeg->SetOrderStatus(orderStatus);

	if(orderStatus == protoc::ALL_TRADED)
	{
		LEG_STATUS currentLegStatus = m_pLeg->GetStatus();
		if(currentLegStatus == IS_OPENING)
		{
			// opening position succeeded
			m_pLeg->SetStatus(OPENED);
		}
		else if(currentLegStatus == IS_CLOSING)
		{
			// closing position succeeded
			m_pLeg->SetStatus(CLOSED);
		}
		else
		{
			assert(false);
			logger.Warning("Unexpected leg status changing");		
		}
	}

	m_pLeg->SetMessage(pOrder->statusmsg());

	m_order = pOrder;
}

void COrderItem::UpdateTrade( protoc::Trade* pTrade )
{
	if(pTrade->offsetflag() == protoc::OF_OPEN)
	{
		m_pLeg->SetCost(pTrade->price());
	}
}


//////////////////////////////////////////////////////////////////////////
// Below is COrderRepository implementation
COrderRepository::COrderRepository(void)
{
}


COrderRepository::~COrderRepository(void)
{
}

void COrderRepository::AddOrderItem( CLeg* pleg )
{
	OrderItemPtr orderItem(new COrderItem(pleg));
	m_orderItems.insert(OrderItemPair(orderItem->GetOrderRef(), orderItem));
}

COrderItem* COrderRepository::GetOrderItem( const std::string& orderRef )
{
	OrderItemMapIter foundIter = m_orderItems.find(orderRef);
	if(foundIter != m_orderItems.end())
		return (foundIter->second).get();
	else
		return NULL;
}

void COrderRepository::RemoveOrderItem( const std::string& orderRef )
{
	OrderItemMapIter foundIter = m_orderItems.find(orderRef);
	if(foundIter != m_orderItems.end())
		m_orderItems.erase(foundIter);
}

int COrderRepository::GetOrderItemCount()
{
	return m_orderItems.size();
}

