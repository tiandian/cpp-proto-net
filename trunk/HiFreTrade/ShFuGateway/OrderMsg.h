#pragma once
#include "message.h"
#include "EntityStructs.h"
#include "ThostFtdcUserApiStruct.h"

class COrderMsg :
	public CMessage
{
public:
	COrderMsg(void);
	~COrderMsg(void);

	const OrderInfo* InnerStruct() { return &m_innerData; }

	void SetData(CThostFtdcOrderField* pOrder);

private:

	int GetEntryExitType(char direction, char offsetFlag);

	int GetOrderStatus(char submitStatus, char orderStatus);

	OrderInfo m_innerData;
};

