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
	: m_conn(conn)
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

	{
		boost::unique_lock< boost::mutex > lock(m_mutex);
		m_conn->async_write(msg, data, boost::bind(&RemoteClient::WriteCompleted, this));
		m_condSocketInUse.wait(lock);
	}
}

void RemoteClient::WriteCompleted()
{ 
	/*boost::lock_guard<boost::mutex> lock(m_mutex);
	logger.Info("Send hahahaha");*/
	m_condSocketInUse.notify_one();
}

void RemoteClient::ReadCompleted(MSG_TYPE msg, std::string& out_data)
{
	boost::lock_guard<boost::mutex> lock(m_mutex);
	m_condSocketInUse.notify_one();
}

void RemoteClient::GetReady()
{
	{
		m_data = "hahahaha";
		//boost::unique_lock< boost::mutex > lock(m_mutex);
		//m_conn->async_read(boost::bind(&RemoteClient::ReadCompleted, this, _1, _2));
		m_conn->async_write(QUOTE, m_data, boost::bind(&RemoteClient::handle_write, this,
									boost::asio::placeholders::error, m_conn));
		//m_condSocketInUse.wait(lock);
		//logger.Info("Send hahahaha");
	}
}


