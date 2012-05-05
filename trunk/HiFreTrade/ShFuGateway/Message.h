#pragma once

enum MsgType
{
	//TEXT,
	QUOTE,
	OPERATION_RECORD,
	TIME_AND_SALES
};

class CMessage
{
public: 
	CMessage(MsgType msgType):
	  m_type(msgType)
	{
	}

	virtual ~CMessage()
	{
	}

	MsgType GetType() 
	{ 
		return m_type; 
	}

private:
	MsgType m_type;
};