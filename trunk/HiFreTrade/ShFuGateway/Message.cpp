#include "StdAfx.h"
#include "Message.h"

void SafeStringCopy(char * destination, const char * source, int destCapacity)
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

CMessage::CMessage( MsgType msgType ) :
m_type(msgType)
{

}

CMessage::~CMessage()
{

}
