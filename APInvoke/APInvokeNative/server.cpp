#include "server.hpp"
#include <boost/bind.hpp>


server::server(const std::string& address, const std::string& port)
  : io_service_(),
    acceptor_(io_service_),
    new_connection_()
{

  // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
  boost::asio::ip::tcp::resolver resolver(io_service_);
  boost::asio::ip::tcp::resolver::query query(address, port);
  boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();

  start_accept();
}

void server::run()
{
  // The io_service::run() call will block until all asynchronous operations
  // have finished. While the server is running, there is always at least one
  // asynchronous operation outstanding: the asynchronous accept call waiting
  // for new incoming connections.
  running_thread_ = boost::thread(boost::bind(&boost::asio::io_service::run, &io_service_));
}

void server::start_accept()
{
  new_connection_.reset(new connection(io_service_));
  acceptor_.async_accept(new_connection_->socket(),
      boost::bind(&server::handle_accept, this,
        boost::asio::placeholders::error));
}

void server::handle_accept(const boost::system::error_code& e)
{
  // Check whether the server was stopped by a signal before this completion
  // handler had a chance to run.
  if (!acceptor_.is_open())
  {
    return;
  }

  if (!e)
  {
    //connection_manager_.start(new_connection_);
	
	try
	{
		m_acceptCallback(new_connection_);
	}
	catch(...)
	{}
  }

  start_accept();
}

void server::stop()
{
  // The server is stopped by cancelling all outstanding asynchronous
  // operations. Once all operations have finished the io_service::run() call
  // will exit.
  acceptor_.close();
  //connection_manager_.stop_all();

  running_thread_.join();
}

