#pragma once

#include "Leg.h"
#include "protobuf_gen/trade.pb.h"

#include <string>
#include <map>
#include <boost/smart_ptr.hpp>

typedef boost::shared_ptr<protoc::Order> OrderPtr;

class COrderItem
{
public:
	COrderItem(CLeg* pleg):m_pLeg(pleg)
	{}
	~COrderItem(){}

	const std::string& GetOrderRef();
	
	protoc::Order* GetOrder();
	void SetOrder(OrderPtr& pOrder);

	CLeg* GetLeg(){ return m_pLeg; }

	void UpdateTrade( protoc::Trade* pTrade);

private:
	OrderPtr m_order;
	CLeg* m_pLeg;
};

typedef boost::shared_ptr<COrderItem> OrderItemPtr;

class COrderRepository
{
public:
	COrderRepository(void);
	~COrderRepository(void);

	void AddOrderItem(CLeg* pleg);

	COrderItem* GetOrderItem(const std::string& orderRef);

	void RemoveOrderItem(const std::string& orderRef);

	int GetOrderItemCount();


private:
	typedef std::map< std::string, OrderItemPtr> OrderItemMap;
	typedef std::pair< std::string, OrderItemPtr > OrderItemPair;
	typedef OrderItemMap::iterator OrderItemMapIter;

	OrderItemMap m_orderItems;

};

