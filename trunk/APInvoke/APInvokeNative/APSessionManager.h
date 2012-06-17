#pragma once

#include "APInvokeNative.h"
#include "connection.h"
#include "server.h"
#include "../Transmission/gen/packet.pb.h"

#include <string>
#include <map>
#include <boost/smart_ptr.hpp>
#include <boost/threadpool.hpp>

using namespace std;

class APSessionManager;

class APSession : public Session
{
public:
	APSession(const string& sid, const connection_ptr& conn):
		m_isContinuousReading(false),
		m_isConnected(false),
		m_pSessionMgr(NULL),
		m_callbackRspHandler(NULL)
	{
		m_sessionId = sid;
		m_conn = conn;
	}

	~APSession(){ Close(); }

	const string& SessionId() { return m_sessionId; }

	bool IsConnected() { return m_isConnected; }

	const string& GetIPAddress() { return m_ipAddr; }

	void BeginCallback(const string& method, const string& callbackReqData);

	void RegisterCallback(SessionCallback* callbackRsp);

	void GetReady(APSessionManager* sessionMgr);

	template<typename T>
	void BeginSendMessage(MSG_TYPE type, T* pT)
	{
		std::string data;
		pT->SerializeToString(&data);
		BeginWrite(type, data);
	}

	void BeginWrite(MSG_TYPE msg, string& data);

	void OnWriteCompleted(const boost::system::error_code& e, std::size_t bytes_transferred);

	void BeginRead();

	void OnReadCompleted(const boost::system::error_code& e, MSG_TYPE msg, const string& data);

	void Close();

private:

	void OnSocketError(const boost::system::error_code& e);
	void OnConnectionEstablished();

	connection_ptr m_conn;
	string m_sessionId;
	string m_ipAddr;
	bool m_isConnected;
	APSessionManager* m_pSessionMgr;
	bool m_isContinuousReading;
	SessionCallback* m_callbackRspHandler;
};

typedef boost::shared_ptr<APSession> SessionPtr;
typedef boost::shared_ptr<AP::Request> RequestPtr;
typedef boost::shared_ptr<AP::CallbackRsp> CallBkRspPtr;

class APSessionManager : public SessionManager
{
public:
	APSessionManager(void);
	virtual ~APSessionManager(void);

	void RegisterHandler(SessionManagerHandler* handler);

	bool Listen(unsigned int nPort);

	void Close();

	APSession* GetSession(const string& sessionId);

	void HandleError(const string& sessionId, const boost::system::error_code& e);

	void HandleRequest(const string& sessionId, const RequestPtr& request);

	void RaiseConnected(Session* session) 
	{
		if(m_callback != NULL) m_callback->OnConnected(session);
	}
	void RaiseDisconnected(Session* session)
	{
		if(m_callback != NULL) m_callback->OnDisconnected(session);
	}
	void RaiseError(Session* session, const string& errorMsg)
	{
		if(m_callback != NULL) m_callback->OnError(session, errorMsg);
	}

private:
	void OnClientAccepted(connection_ptr conn);

	void OnRequest(string sessionId, const RequestPtr& request);


	SessionManagerHandler* m_callback;

	boost::shared_ptr<server> m_server;

	std::map<string, SessionPtr> m_clientMap;
	typedef std::map<string, SessionPtr>::iterator ClientMapIter;

	typedef boost::threadpool::thread_pool<	boost::threadpool::task_func, 
											boost::threadpool::fifo_scheduler, 
											boost::threadpool::static_size, 
											boost::threadpool::resize_controller, 
											boost::threadpool::immediately> ThreadPool;
	ThreadPool m_threadPool;
};

