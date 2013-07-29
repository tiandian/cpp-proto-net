#include "StdAfx.h"
#include "MultiLegOrder.h"


PTEntity::MultiLegOrder::MultiLegOrder( const trade::MultiLegOrder* pEntity )
{
	OrderId = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntity->orderid().c_str());
	PortfolioId = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntity->portfolioid().c_str());
	Quantity = pEntity->quantity();
	
	OpenOrderId = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntity->openorderid().c_str());
	OpenDate = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntity->opendate().c_str());
	Reason = static_cast<SubmitReason>(pEntity->reason());
	HasWarn = pEntity->haswarn();
	StatusMsg = Marshal::PtrToStringAnsi((IntPtr) (char *) pEntity->statusmsg().c_str());
	Offset = static_cast<MlOrderOffset>(pEntity->offset());

	// legs
	List<Order^> ^legOrders = gcnew List<Order^>();
	int legCount = pEntity->legs_size();
	for(int i = 0; i < legCount; ++i)
	{
		Order^ o = gcnew Order(&(pEntity->legs(i)));
		legOrders->Add(o);
	}
	Legs = legOrders->ToArray();
}
