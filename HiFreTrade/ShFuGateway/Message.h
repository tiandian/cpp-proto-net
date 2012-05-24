#pragma once

enum MsgType
{
	//TEXT,
	QUOTE,
	OPERATION_RECORD,
	TIME_AND_SALES,
	ACCOUNT_INFO,
	POSITION_DETAIL,
	ORDER
};

class CMessage
{
public: 
	CMessage(MsgType msgType);

	virtual ~CMessage();

	MsgType GetType() 
	{ 
		return m_type; 
	}

private:
	MsgType m_type;
};

void SafeStringCopy(char * destination, const char * source, int destCapacity);