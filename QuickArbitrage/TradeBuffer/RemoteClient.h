#pragma once

#include "clientbase.h"
#include "connection.h"

class RemoteClient :
	public ClientBase
{
public:
	RemoteClient(connection_ptr conn);
	~RemoteClient(void);

private:
	connection_ptr m_conn;
};

