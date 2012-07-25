#pragma once

#include "../Entity/gen/cpp/trade.pb.h"

class COrderProcessor
{
public:
	COrderProcessor(void);
	~COrderProcessor(void);

	void OpenOrder(){}
	void CloseOrder(){}
	void CancelOrder(){}
};

