#include "StdAfx.h"
#include "RemoteClient.h"
#include "LogManager.h"
#include "ConnectionManager.h"
#include "protobuf_gen/login.pb.h"

#include <boost/bind.hpp>
#include <boost/tuple/tuple.hpp>

extern CLogManager logger;

#define QUALIFIED_USER "xixihaha"
#define QUALIFIED_PWD "thisispwd"

MSG_TYPE MakeTransmitPacket(boost::shared_ptr<CTP::Quote>& pQuote, std::string* data)
{
	pQuote->SerializeToString(data);
	return QUOTE;
}

RemoteClient::RemoteClient(std::string& sessionId, connection_ptr conn)
	: m_conn(conn), m_isContinuousReading(false), m_pConnMgr(NULL)
{
	m_sessionId = sessionId;
	m_ipAddr = m_conn->socket().remote_endpoint().address().to_string();
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
		OnSocketError(e);
	}
	else
	{
		switch (msg)
		{	
		case REQ_LOGIN:
			{
				protoc::ReqLogin reqLogin;
				reqLogin.ParseFromString(data);
				OnLogin(reqLogin.username(), reqLogin.password());
			}
			break;
		case REQ_SUBSCRIBE:
			break;
		case REQ_UNSUBSCRIBE:
			break;

		}

		//if(m_isContinuousReading)
		//	BeginRead();
	}
}

void RemoteClient::GetReady(CConnectionManager* pConnMgr)
{
	m_pConnMgr = pConnMgr;
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

void RemoteClient::OnLogin( const std::string& username, const std::string& password )
{
	std::ostringstream oss;
	oss << "Client request login with username('" << username << "') and password('" << password << "')";
	logger.Info(oss.str());

	protoc::RspLogin rsp;
	if(username == QUALIFIED_USER && password == QUALIFIED_PWD)
	{
		// approve login
		rsp.set_succ(true);
		rsp.set_session(m_sessionId);
	}
	else
	{
		// refuse
		rsp.set_succ(false);
		rsp.set_session(m_sessionId);
	}
	std::string rsp_data;
	bool succ = rsp.SerializeToString(&rsp_data);
	assert(succ == true);
	BeginWrite(RSP_LOGIN, rsp_data);
}

void RemoteClient::OnSubscribe( const std::vector<std::string>& symbols )
{

}

void RemoteClient::OnUnSubscribe( const std::vector<std::string>& symbols )
{

}

void RemoteClient::OnSocketError( const boost::system::error_code& e )
{
	m_conn->socket().get_io_service().post(boost::bind(&CConnectionManager::HandleError, m_pConnMgr, m_sessionId, e));
}


