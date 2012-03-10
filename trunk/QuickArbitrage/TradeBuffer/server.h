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
	{
		io_service_thread = boost::thread(&server::io_service_run_proc, this, port);
	}
	~server()
	{}

	void set_accept_handler(AcceptHandler handler)
	{
		m_acceptCallback = handler;
	}

	void stop()
	{
		if(p_acceptor_ != NULL)
		{
			//p_acceptor_->close();
			p_acceptor_.reset();
			p_io_service_->stop();
			io_service_thread.join();
			p_io_service_.reset();
		}
	}

private:

	void io_service_run_proc(unsigned short port)
	{ 
		p_io_service_ = boost::shared_ptr<boost::asio::io_service>(new boost::asio::io_service);

		boost::asio::ip::tcp::acceptor* pAcceptor = new boost::asio::ip::tcp::acceptor(
														*p_io_service_, 
														boost::asio::ip::tcp::endpoint(
															boost::asio::ip::tcp::v4(), port));
		p_acceptor_ = boost::shared_ptr<boost::asio::ip::tcp::acceptor>(pAcceptor);

		async_wait_client();

		try
		{
			p_io_service_->run(); 
		}
		catch (std::exception& e)
		{
			std::cout << e.what();
		}
		
	}

	void async_wait_client()
	{
		// Start an accept operation for a new connection.
		connection_ptr new_conn(new connection(p_acceptor_->get_io_service()));
		p_acceptor_->async_accept(new_conn->socket(),
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

		// if acceptor is closed, don't need to wait any more
		if(e.value() != 995)
		{
			async_wait_client();
		}
	}

	// The acceptor object used to accept incoming socket connections.
	boost::shared_ptr<boost::asio::ip::tcp::acceptor> p_acceptor_;
	boost::shared_ptr<boost::asio::io_service> p_io_service_;
	boost::thread io_service_thread;

	AcceptHandler m_acceptCallback;
};