#include "StdAfx.h"
#include "RemoteClient.h"
#include "LogManager.h"
#include "ConnectionManager.h"
#include "OrderManager.h"
#include "protobuf_gen/login.pb.h"
#include "protobuf_gen/subscribe.pb.h"

#include <set>
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/tuple/tuple.hpp>

extern CLogManager logger;
extern COrderManager g_orderMgr;

#define QUALIFIED_USER "xixihaha"
#define QUALIFIED_PWD "thisispwd"

RemoteClient::RemoteClient(std::string& sessionId, connection_ptr conn)
	: m_conn(conn), m_isContinuousReading(false), m_pConnMgr(NULL),m_tradeLoggedin(false)
{
	m_sessionId = sessionId;
	m_ipAddr = m_conn->socket().remote_endpoint().address().to_string();
}


RemoteClient::~RemoteClient(void)
{
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
			{
				protoc::ReqSubscribe reqSub;
				reqSub.ParseFromString(data);
				
				std::vector<std::string> vecSymbols;
				vecSymbols.resize(reqSub.symbols().size());
				std::copy(reqSub.symbols().begin(), reqSub.symbols().end(), vecSymbols.begin());
				
				OnSubscribe(vecSymbols);
			}
			break;
		case REQ_UNSUBSCRIBE:
			{
				protoc::ReqUnsubscribe reqUnSub;
				reqUnSub.ParseFromString(data);

				std::vector<std::string> vecSymbols;
				vecSymbols.resize(reqUnSub.symbols().size());
				std::copy(reqUnSub.symbols().begin(), reqUnSub.symbols().end(), vecSymbols.begin());

				OnUnSubscribe(vecSymbols);
			}
			break;

		}

		if(m_isContinuousReading)
			BeginRead();
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

	m_brokerId = "0240";
	m_userId = username;
	g_orderMgr.Register(this, m_brokerId, m_userId, const_cast<std::string&>(password));
}


void RemoteClient::OnRegisterResult( bool succ, std::string& errMsg )
{
	boost::shared_ptr<protoc::RspLogin> rsp(new protoc::RspLogin());
	
	m_tradeLoggedin = succ;

	if(succ)
	{
		// approve login
		rsp->set_succ(true);
		rsp->set_session(m_sessionId);
	}
	else
	{
		// refuse
		rsp->set_succ(false);
		rsp->set_session(m_sessionId);
	}

	EnqueueMessage(RSP_LOGIN, rsp);
}

void RemoteClient::OnSubscribe( std::vector<std::string>& symbols )
{
	boost::shared_ptr<protoc::RspSubscribe> rspSub(new protoc::RspSubscribe());

	try{
		Subscribe(symbols);
		rspSub->set_succ(true);
	}
	catch(...)
	{
		rspSub->set_succ(false);
	}
	
	EnqueueMessage(RSP_SUBSCRIBE, rspSub);
}

void RemoteClient::OnUnSubscribe( std::vector<std::string>& symbols )
{
	boost::shared_ptr<protoc::RspUnsubscribe> rspUnsub(new protoc::RspUnsubscribe());

	try{
		rspUnsub->set_succ(true);

		if(symbols.size() == 0)
		{
			UnSubscribe();
		}
		else
		{
			std::set<std::string> UnsubSet(symbols.begin(), symbols.end());
			std::vector<std::string>& currentSymbols = GetSymbols();
			std::set<std::string> OrigSet(currentSymbols.begin(), currentSymbols.end());

			std::vector<std::string> ChangeToSymbols;
			ChangeToSymbols.resize(currentSymbols.size());
			std::vector<std::string>::iterator lastIter = std::set_difference(OrigSet.begin(), OrigSet.end(), 
													UnsubSet.begin(), UnsubSet.end(), ChangeToSymbols.begin());
			ChangeToSymbols.resize(lastIter - ChangeToSymbols.begin());
			if(ChangeToSymbols.size() < currentSymbols.size())
			{
				// subscring symbols is indeed changed
				Subscribe(ChangeToSymbols);
			}
		}
	}
	catch(...)
	{
		rspUnsub->set_succ(false);
	}
	
	EnqueueMessage(RSP_UNSUBSCRIBE, rspUnsub);
}

void RemoteClient::OnSocketError( const boost::system::error_code& e )
{
	m_conn->socket().get_io_service().post(boost::bind(&CConnectionManager::HandleError, m_pConnMgr, m_sessionId, e));
}

void RemoteClient::ProcessMessage( MSG_TYPE type, void* pData )
{
	switch(type)
	{
	case RSP_LOGIN:
		{
			protoc::RspLogin* pLogin = static_cast<protoc::RspLogin*>(pData);
			BeginSendMessage(type, pLogin);
		}
		break;
	case RSP_SUBSCRIBE:
		{
			protoc::RspSubscribe* pSub = static_cast<protoc::RspSubscribe*>(pData);
			BeginSendMessage(type, pSub);
		}
		break;
	case RSP_UNSUBSCRIBE:
		{
			protoc::RspUnsubscribe* pUnSub = static_cast<protoc::RspUnsubscribe*>(pData);
			BeginSendMessage(type, pUnSub);
		}
		break;
	}
}

void RemoteClient::Close()
{
	UnSubscribe(); // Unsubscribe quote from market

	if(m_tradeLoggedin)
		g_orderMgr.Unregister(m_brokerId, m_userId);

	m_conn->close();
}



