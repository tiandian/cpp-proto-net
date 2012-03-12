#include "StdAfx.h"
#include "RemoteClient.h"
#include "LogManager.h"

#include <boost/bind.hpp>
#include <boost/tuple/tuple.hpp>

extern CLogManager logger;

MSG_TYPE MakeTransmitPacket(boost::shared_ptr<CTP::Quote>& pQuote, std::string* data)
{
	pQuote->SerializeToString(data);
	return QUOTE;
}

RemoteClient::RemoteClient(std::string& sessionId, connection_ptr conn)
	: m_conn(conn), m_isContinuousReading(false)
{
	m_sessionId = sessionId;
}


RemoteClient::~RemoteClient(void)
{
}

void RemoteClient::ProcessQuote( boost::shared_ptr<CTP::Quote>& pQuote )
{
	std::string data;
	MSG_TYPE msg = MakeTransmitPacket(pQuote, &data);
	BeginWrite(msg, data);	// may block some time before the last sent done.
}

void RemoteClient::WriteCompleted(const boost::system::error_code& e, std::size_t bytes_transferred)
{ 
	logger.Trace("Write completed");
}

void RemoteClient::OnDataReceived(const boost::system::error_code& e, MSG_TYPE msg, std::string& data)
{
	if(e)
	{
		logger.Trace(e.message());
	}
	else
	{
		switch (msg)
		{	
		case REQ_LOGIN:
			break;
		case REQ_SUBSCRIBE:
			break;
		case REQ_UNSUBSCRIBE:
			break;

		}
	}

	if(m_isContinuousReading)
		BeginRead();

}

void RemoteClient::GetReady()
{
	m_isContinuousReading = true;
	BeginRead();
}

// Actually continuous reading
void RemoteClient::BeginRead()
{
	m_conn->async_read(boost::bind(&RemoteClient::OnDataReceived, this, _1, _2, _3));
}

void RemoteClient::BeginWrite(MSG_TYPE msg, std::string& data)
{
	m_conn->async_write(msg, data, boost::bind(&RemoteClient::WriteCompleted, this, _1, _2));
}

void RemoteClient::OnLogin( std::string& username, std::string& password )
{

}

void RemoteClient::OnSubscribe( const std::vector<std::string>& symbols )
{

}

void RemoteClient::OnUnSubscribe( const std::vector<std::string>& symbols )
{

}


