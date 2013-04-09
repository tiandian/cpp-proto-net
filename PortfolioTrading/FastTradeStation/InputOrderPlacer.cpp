#include "InputOrderPlacer.h"
#include "Portfolio.h"
#include "OrderProcessor2.h"

bool CInputOrderPlacer::OnEnter( ORDER_STATE state, COrderEvent* transEvent, ORDER_STATE lastState )
{
	return false;
}

void CInputOrderPlacer::Do()
{

}

void CInputOrderPlacer::AdjustQuantity( int qty )
{

}

void CInputOrderPlacer::CancelOrder( trade::Order* pOrd )
{

}

void CInputOrderPlacer::RaiseMultiLegOrderEvent( COrderEvent* orderEvent )
{

}

void CInputOrderPlacer::ModifyOrderPrice()
{

}

void CInputOrderPlacer::OnSubmittingOrder()
{

}

void CInputOrderPlacer::OnOrderUpdate( trade::Order* pOrd )
{

}

void CInputOrderPlacer::OnOrderPlaceFailed( COrderEvent* pOrdEvent )
{

}

void CInputOrderPlacer::OnOrderAccept( trade::Order* pOrd )
{

}

void CInputOrderPlacer::OnPending( trade::Order* pOrd )
{

}

void CInputOrderPlacer::OnCanceling( COrderEvent* transEvent )
{

}

void CInputOrderPlacer::OnOrderComplete( trade::Order* pOrd )
{

}

void CInputOrderPlacer::SetPrice( double price )
{
	m_pInputOrder->set_limitprice(price);
}

void CInputOrderPlacer::SetDirection( trade::TradeDirectionType direction )
{
	m_pInputOrder->set_direction(direction);
}
