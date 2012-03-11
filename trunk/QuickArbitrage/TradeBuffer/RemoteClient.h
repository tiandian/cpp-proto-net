#pragma once

#include "clientbase.h"
#include "connection.h"

#include <string>
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

	/// Handle completion of a read operation.
	void handle_read(const boost::system::error_code& e)
	{

	}

	/// Handle completion of a write operation.
	void handle_write(const boost::system::error_code& e, connection_ptr conn)
	{
		// Nothing to do. The socket will be closed automatically when the last
		// reference to the connection object goes away.
		WriteCompleted();
	}


	void WriteCompleted();
	void ReadCompleted(MSG_TYPE msg, std::string& out_data);

private:
	connection_ptr m_conn;
	std::string m_sessionId;
	boost::mutex m_mutex;
	boost::condition_variable m_condSocketInUse;

	std::string m_data;
};

