#include "StdAfx.h"
#include "ClientManager.h"
#include "globalmembers.h"
#include "../Entity/gen/cpp/message.pb.h"

#include <boost/format.hpp>
#include <boost/bind.hpp>

CClientManager::CClientManager(void)
{
	InitializeReqTranslators();
}


CClientManager::~CClientManager(void)
{
}

void CClientManager::OnConnected( Session* session )
{
	std::string info = boost::str(boost::format("Client(%s) connected.") % session->SessionId().c_str());
	logger.Info(info);

	ClientPtr client(new CClientAgent);
	client->SetSession(session);
	m_clients.insert(std::make_pair(session->SessionId(), client));
}

void CClientManager::OnDisconnected( Session* session )
{
	std::string info = boost::str(boost::format("Client(%s) disconnected.") % session->SessionId().c_str());
	logger.Info(info);

	CClientAgent* pClntAgent = GetClient(session->SessionId());
	if(pClntAgent != NULL)
		pClntAgent->SetSession(NULL);
}

void CClientManager::OnError( Session* session, const string& errorMsg )
{
	std::string outputErr = boost::str(boost::format("Client(%s) encounter error - %s.") 
		% session->SessionId().c_str() % errorMsg.c_str());
	logger.Info(outputErr);
}

void CClientManager::DispatchPacket( const string& sessionId, const string& method, const string& in_data, string& out_data )
{
	CClientAgent* pClntAgent = GetClient(sessionId);
	if(pClntAgent != NULL)
	{
		ReqTranslatorMapIter transIter = m_reqTransMap.find(method);
		if(transIter != m_reqTransMap.end())
		{
			(transIter->second)(pClntAgent, in_data, out_data);
		}
	}
}

CClientAgent* CClientManager::GetClient( const string& sessionId )
{
	ClientMapIter clntIter = m_clients.find(sessionId);
	if(clntIter != m_clients.end())
	{
		return (clntIter->second).get();
	}
	else
		return NULL;
}

void CClientManager::InitializeReqTranslators()
{
	m_reqTransMap.insert(make_pair("QuoteConnect", boost::bind(&CClientManager::QuoteConnect, this, _1, _2, _3)));
}

void CClientManager::QuoteConnect( CClientAgent* pClientAgent, const string& in_data, string& out_data )
{
	entity::ConnectParam connParam;
	connParam.ParseFromString(in_data);

	boost::tuple<bool, string> ret = pClientAgent->QuoteConnect(connParam.quoteaddress(), connParam.streamfolder());

	entity::OperationReturn operRet;
	operRet.set_success(boost::get<0>(ret));
	operRet.set_errormessage(boost::get<1>(ret));

	operRet.SerializeToString(&out_data);
}


