#pragma once
#include "message.h"
#include "EntityStructs.h"

class COperationRecordData :
	public CMessage
{
public:
	COperationRecordData(void);
	~COperationRecordData(void);

	int RecordId(){ return m_innerData.iRecordId; }
	void SetRecordId(int recordId){ m_innerData.iRecordId = recordId; }

	const char* Symbol() { return m_innerData.caSymbol; }
	void SetSymbol(const char* symbol) 
	{
		SafeStringCopy(m_innerData.caSymbol, symbol, SYMBOL_LEN);
	}

	const char* RectPeriodBegin() {return m_innerData.caRectPeriodBegin;}
	void SetRectPeriodBegin(const char* periodBeginTime)
	{
		SafeStringCopy(m_innerData.caRectPeriodBegin, periodBeginTime, UPDATE_TIME_LEN);
	}

	const char* RectPeriodEnd() {return m_innerData.caRectPeriodEnd;}
	void SetRectPeriodEnd(const char* periodEndTime)
	{
		SafeStringCopy(m_innerData.caRectPeriodEnd, periodEndTime, UPDATE_TIME_LEN);
	}

	double UpperBoundary(){return m_innerData.dUpperBoundary;}
	void SetUpperBoundary(double upperBoundary){m_innerData.dUpperBoundary = upperBoundary;}

	double LowerBoundary(){ return m_innerData.dLowerBoundary; }
	void SetLowerBoundary(double lowerBoundary) { m_innerData.dLowerBoundary = lowerBoundary; }

	double Range() { return m_innerData.dRange; }
	void SetRange(double range){ m_innerData.dRange = range; }

	int Direction(){ return m_innerData.iDirection; }
	void SetDirection(int direction){ m_innerData.iDirection = direction; }

	double EntryPoint() { return m_innerData.dEntryPoint; }
	void SetEntryPoint(double entryPoint) { m_innerData.dEntryPoint = entryPoint; }

	const char* EntryTime() { return m_innerData.caEntryTime; }
	void SetEntryTime(const char* entryTime)
	{
		SafeStringCopy(m_innerData.caEntryTime, entryTime, UPDATE_TIME_LEN);
	}

	int EntryReason() {return m_innerData.iEntryReason; }
	void SetEntryReason(int entryReason){ m_innerData.iEntryReason = entryReason; }

	int EntryType() { return m_innerData.iEntryType; }
	void SetEntryType(int entryType) { m_innerData.iEntryType = entryType; }

	int EntryQuantity() { return m_innerData.iEntryQuantity; }
	void SetEntryQuantity(int quantity) { m_innerData.iEntryQuantity = quantity; }

	int EntryStatus(){ return m_innerData.iEntryStatus; }
	void SetEntryStatus(int status){ m_innerData.iEntryStatus = status; }

	double ExitPoint() { return m_innerData.dExitPoint; }
	void SetExitPoint(double exitPoint) { m_innerData.dExitPoint = exitPoint; }

	const char* ExitTime() { return m_innerData.caExitTime; }
	void SetExitTime(const char* exitTime)
	{
		SafeStringCopy(m_innerData.caExitTime, exitTime, UPDATE_TIME_LEN);
	}

	int ExitReason() {return m_innerData.iExitReason; }
	void SetExitReason(int exitReason){ m_innerData.iExitReason = exitReason; }

	int ExitType() { return m_innerData.iExitType; }
	void SetExitType(int exitType) { m_innerData.iExitType = exitType; }

	int ExitQuantity() { return m_innerData.iExitQuantity; }
	void SetExitQuantity(int quantity) { m_innerData.iExitQuantity = quantity; }

	int ExitStatus(){ return m_innerData.iExitStatus; }
	void SetExitStatus(int status){ m_innerData.iExitStatus = status; }

	double ProfitLoss() { return m_innerData.ProfitLoss; }
	void SetProfitLoss(double pnl){ m_innerData.ProfitLoss = pnl;}

	const OperationRecord* InnerStruct() { return &m_innerData; }

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

	OperationRecord m_innerData;
};

