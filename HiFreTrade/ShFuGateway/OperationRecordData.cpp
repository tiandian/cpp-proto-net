#include "StdAfx.h"
#include "OperationRecordData.h"


COperationRecordData::COperationRecordData(void):
CMessage(OPERATION_RECORD)
{
	memset(&m_innerData, 0, sizeof(m_innerData));
}


COperationRecordData::~COperationRecordData(void)
{
}
