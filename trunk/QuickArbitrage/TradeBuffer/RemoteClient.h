#pragma once

#include "clientbase.h"
#include "connection.h"
#include "OrderProcessor.h"

#include <string>
#include <vector>
#include <boost/bind.hpp>
#include <boost/thread/locks.hpp>

class CConnectionManager;

class RemoteClient :
	public ClientBase
{
public:
	RemoteClient(std::string& sessionId, connection_ptr conn);
	~RemoteClient(void);

	std::string& GetSessionID() { return m_sessionId; }

	void GetReady(CConnectionManager* pConnMgr);

	const std::string& GetIPAddress() { return m_ipAddr; }

	void Close() 
	{
		UnSubscribe(); // Unsubscribe quote from market
		m_conn->close(); 
	}

protected:

	virtual void ProcessMessage(MSG_TYPE type, void* pData);

	virtual void ProcessQuote(CTP::Quote* pQuote)
	{
		BeginSendMessage(QUOTE, pQuote);
	}

	void WriteCompleted(const boost::system::error_code& e, std::size_t bytes_transferred);

	void OnDataReceived(const boost::system::error_code& e, MSG_TYPE msg, std::string& data);

	void BeginRead();

	void BeginWrite(MSG_TYPE msg, std::string& data);

	template<typename T>
	void BeginSendMessage(MSG_TYPE type, T* pT)
	{
		std::string data;
		pT->SerializeToString(&data);
		BeginWrite(type, data);
	}

	// business associated
	void OnLogin(const std::string& username, const std::string& password);

	void OnSubscribe(std::vector<std::string>& symbols);

	void OnUnSubscribe(std::vector<std::string>& symbols);

private:

	void OnSocketError(const boost::system::error_code& e);

	connection_ptr m_conn;
	std::string m_sessionId;
	std::string m_ipAddr;

	bool m_isContinuousReading;

	CConnectionManager* m_pConnMgr;

	COrderProcessor m_orderProcessor;
};

