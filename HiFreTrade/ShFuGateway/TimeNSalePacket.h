#pragma once
#include "message.h"
#include "EntityStructs.h"

class CTimeNSalePacket :
	public CMessage
{
public:
	CTimeNSalePacket(void);
	~CTimeNSalePacket(void);

	const char* Symbol() { return m_innerData.caSymbol; }
	void SetSymbol(const char* symbol) 
	{
		SafeStringCopy(m_innerData.caSymbol, symbol, SYMBOL_LEN);
	}

	const char* TimeStamp() {return m_innerData.caTimeStamp;}
	void SetTimeStamp(const char* timestamp)
	{
		SafeStringCopy(m_innerData.caTimeStamp, timestamp, UPDATE_TIME_LEN);
	}

	double Price(){ return m_innerData.dPrice;}
	void SetPrice(double px){ m_innerData.dPrice = px; }

	int Quantity(){ return m_innerData.iQuantity; }
	void SetQuantity(int quantity){ m_innerData.iQuantity = quantity; }

	int Direction(){ return m_innerData.iDirection; }
	void SetDirection(int direction){ m_innerData.iDirection = direction; }

	const TimeNSalesData* InnerStruct() { return &m_innerData; }

private:
	static void SafeStringCopy(char * destination, const char * source, int destCapacity)
	{
		int len = strlen(source);
		if(len < destCapacity)
		{
			strcpy(destination, source);
		}
		else
		{
			strncpy(destination, source, destCapacity - 1);
		}
	}

	TimeNSalesData m_innerData;
};

