#pragma once

#include "connection.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

typedef boost::function<void(connection_ptr)> AcceptHandler;

class server
{
public:
	server(unsigned short port)
		: acceptor_(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
	{
		async_wait_client();

		io_service_thread = boost::thread(&server::io_service_run_proc, this);
	}
	~server()
	{}

	void set_accept_handler(AcceptHandler handler)
	{
		m_acceptCallback = handler;
	}

private:

	void io_service_run_proc()
	{ 
		io_service.run(); 
	}

	void async_wait_client()
	{
		// Start an accept operation for a new connection.
		connection_ptr new_conn(new connection(acceptor_.get_io_service()));
		acceptor_.async_accept(new_conn->socket(),
			boost::bind(&server::handle_accept, this,
			boost::asio::placeholders::error, new_conn));
	}

	/// Handle completion of a accept operation.
	void handle_accept(const boost::system::error_code& e, connection_ptr conn)
	{
		if (!e)
		{
			try
			{
				m_acceptCallback(conn);
			}
			catch(...)
			{}
		}

		async_wait_client();
	}

	/// The acceptor object used to accept incoming socket connections.
	boost::asio::ip::tcp::acceptor acceptor_;
	boost::asio::io_service io_service;
	boost::thread io_service_thread;

	AcceptHandler m_acceptCallback;
};