#include "apsessionmanager.h"

#include <sstream>
#include <boost/bind.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>


SessionManager* SessionManager::Create()
{
	return new APSessionManager;
}

bool HandleConnected(const string& userId, const string& pwd)
{
	return pwd == "APInvoke Connection";
}

APSessionManager::APSessionManager(void):
m_callback(NULL),
m_threadPool(2)
{
}


APSessionManager::~APSessionManager(void)
{
}

bool APSessionManager::Listen( const string& sAddr, const string& sPort )
{
	m_server = boost::shared_ptr<server>(new server(sAddr, sPort));
	m_server->set_accept_handler(boost::bind(&APSessionManager::OnClientAccepted, this, _1));
	m_server->run();
	return true;
}

void APSessionManager::Close()
{
	// close all client first
	for (ClientMapIter cltIter = m_clientMap.begin(); cltIter != m_clientMap.end(); ++ cltIter)
	{
		(cltIter->second)->Close();
	}

	// destory clients
	m_clientMap.clear();

	m_server->stop();

	// destory server
	m_server.reset();
}

void APSessionManager::RegisterHandler( SessionManagerHandler* handler )
{
	m_callback = handler;
}

void APSessionManager::OnClientAccepted( connection_ptr conn )
{
	boost::recursive_mutex::scoped_lock lock(m_mutSessionMap);
	boost::uuids::random_generator gen;
	boost::uuids::uuid uuid_ = gen();
	string sessionId = boost::uuids::to_string(uuid_);

	SessionPtr client(new APSession(sessionId, conn));
	m_clientMap.insert(std::make_pair(sessionId, client));
	client->GetReady(this);
}

void APSessionManager::HandleError( const string& sessionId, const boost::system::error_code& e )
{
	if(e)
	{
		boost::recursive_mutex::scoped_lock lock(m_mutSessionMap);
		// log error
		std::ostringstream oss;
		std::map<std::string, SessionPtr>::iterator foundClnt = m_clientMap.find(sessionId);

		Session* pSession = NULL;

		if(foundClnt != m_clientMap.end())
		{
			oss << "Client (ip:" << (foundClnt->second)->GetIPAddress() << ", sid:" << sessionId <<") encounter error:";
			oss << e.message();
			pSession = (foundClnt->second).get();
		}
		else
		{
			oss << "Error happent to the not existing client : " << e.message();
		}

		RaiseError(pSession, oss.str());

		// handle error
		int eVal = e.value();

		// client close the connection
		if(eVal == 10054 ||			// socket closed: client abnormally shut down
			eVal == 2)				// end of file: client call close
		{
			// close myself
			(foundClnt->second)->Close();

			RaiseDisconnected(pSession);

			// remove from map storage
			m_clientMap.erase(foundClnt);
			
		}
	}
}

void APSessionManager::HandleRequest( const string& sessionId, const RequestPtr& request )
{
	m_threadPool.schedule(boost::bind(&APSessionManager::OnRequest, this, sessionId, request));
}

// Be intentional to copy sessionId
void APSessionManager::OnRequest( string sessionId, const RequestPtr& request )
{
	string outData;
	bool invokeSucc = false;
	string errorMsg = "Encoutered unknown error";
	try
	{
		m_callback->DispatchPacket(sessionId, request->method(), request->param_data(), outData);
		invokeSucc = true;
	}
	catch (exception& ex)
	{
		errorMsg = ex.what();
	}
	catch(...)
	{
		errorMsg = "Encoutered unknown error";
	}
	
	AP::Response resp;
	resp.set_method(request->method());
	resp.set_return_data(outData);
	resp.set_invoke(invokeSucc);
	resp.set_error(errorMsg);

	APSession* pSession = GetSession(sessionId);
	if(pSession != NULL && pSession->IsConnected())
	{
		pSession->BeginSendMessage(RSP, &resp);
	}
}

APSession* APSessionManager::GetSession( const string& sessionId )
{
	boost::recursive_mutex::scoped_lock lock(m_mutSessionMap);
	ClientMapIter iter = m_clientMap.find(sessionId);
	if(iter != m_clientMap.end())
		return (iter->second).get();
	else
		return NULL;
}

void APSession::GetReady( APSessionManager* sessionMgr )
{
	m_pSessionMgr = sessionMgr;
	m_isContinuousReading = true;
	BeginRead();
	m_isConnected = true;
}

void APSession::BeginWrite( MSG_TYPE msg, string& data )
{
	m_conn->async_write(msg, data, boost::bind(&APSession::OnWriteCompleted, this, _1, _2));
}

void APSession::OnWriteCompleted( const boost::system::error_code& e, std::size_t bytes_transferred )
{
	if(e)
	{
		OnSocketError(e);
	}
	else
	{

	}
}

void APSession::BeginRead()
{
	m_conn->async_read(boost::bind(&APSession::OnReadCompleted, this, _1, _2, _3));
}

void APSession::OnReadCompleted( const boost::system::error_code& e, MSG_TYPE msg, const string& data )
{
	if(e)
	{
		OnSocketError(e);
	}
	else
	{
		switch (msg)
		{	
		case CONN:
			{
				AP::Connect reqConn;
				reqConn.ParseFromString(data);
				
				AP::ConnectAck rspConn;
				rspConn.set_success(false);
				rspConn.set_session("");
				rspConn.set_attach_existing(false);

				if(HandleConnected(reqConn.userid(), reqConn.password()))
				{
					// check whether client already existing
					bool attachClient = false;
					if(VerifyClient(reqConn.userid(), reqConn.password(), &attachClient))
					{
						bool connectedSucc = OnConnectionEstablished(reqConn.userid(), attachClient);
						if(connectedSucc)
						{
							rspConn.set_success(true);
							rspConn.set_session(SessionId());
							rspConn.set_attach_existing(attachClient);
						}
					}
				}

				BeginSendMessage(CONN_ACK, &rspConn);
			}
			break;
		case REQ:
			{
				RequestPtr reqMethod( new AP::Request );
				reqMethod->ParseFromString(data);

				m_pSessionMgr->HandleRequest(SessionId(), reqMethod);
			}
			break;
		case CALLBK_RSP:
			{
				CallBkRspPtr callbackRsp (new AP::CallbackRsp);
				callbackRsp->ParseFromString(data);

				m_callbackRspHandler->OnCallbackResponse(callbackRsp->method(), callbackRsp->return_data());
			}
			break;
		}

		if(m_isContinuousReading)
			BeginRead();
	}
}

void APSession::OnSocketError( const boost::system::error_code& e )
{
	if(m_pSessionMgr != NULL && m_isConnected)
	{
		m_conn->fault();
		m_conn->socket().get_io_service().post(boost::bind(&APSessionManager::HandleError, m_pSessionMgr, m_sessionId, e));
	}
}

void APSession::Close()
{
	m_isConnected = false;
	m_conn->close();
}

void APSession::BeginCallback( const string& method, const string& callbackReqData )
{
	AP::CallbackReq callbackReq;
	callbackReq.set_method(method);
	callbackReq.set_param_data(callbackReqData);

	BeginSendMessage(CALLBK_REQ, &callbackReq);
}

void APSession::RegisterCallback( SessionCallback* callbackRsp )
{
	m_callbackRspHandler = callbackRsp;
}

bool APSession::OnConnectionEstablished(const string& clientId, bool attach)
{
	if(m_pSessionMgr != NULL)
	{
		return m_pSessionMgr->RaiseConnected(this, clientId, attach);
		//m_conn->socket().get_io_service().post(boost::bind(&APSessionManager::RaiseConnected, m_pSessionMgr, this, clientId, attach));
	}
	return false;
}

bool APSession::VerifyClient( const string& user, const string& pwd, bool* clientExisting )
{
	if(m_pSessionMgr != NULL)
	{
		return m_pSessionMgr->VerifyClient(user, pwd, clientExisting);
	}
	return false;
}
