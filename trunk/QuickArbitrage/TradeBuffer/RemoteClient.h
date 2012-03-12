#pragma once

#include "clientbase.h"
#include "connection.h"

#include <string>
#include <vector>
#include <boost/thread/locks.hpp>

class RemoteClient :
	public ClientBase
{
public:
	RemoteClient(std::string& sessionId, connection_ptr conn);
	~RemoteClient(void);

	std::string& GetSessionID() { return m_sessionId; }

	void GetReady();

protected:

	void ProcessQuote(boost::shared_ptr<CTP::Quote>& pQuote);

	void WriteCompleted(const boost::system::error_code& e, std::size_t bytes_transferred);

	void OnDataReceived(const boost::system::error_code& e, MSG_TYPE msg, std::string& data);

	void BeginRead();

	void BeginWrite(MSG_TYPE msg, std::string& data);

	// business associated
	void OnLogin(std::string& username, std::string& password);

	void OnSubscribe(const std::vector<std::string>& symbols);

	void OnUnSubscribe(const std::vector<std::string>& symbols);

private:

	connection_ptr m_conn;
	std::string m_sessionId;

	bool m_isContinuousReading;
};

