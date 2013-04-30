#pragma once

#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/noncopyable.hpp>
#include "connection.h"

typedef boost::function<void(connection_ptr)> AcceptHandler;

class server
  : private boost::noncopyable
{
public:
  /// Construct the server to listen on the specified TCP address and port
  explicit server(const std::string& address, const std::string& port);

  /// Run the server's io_service loop.
  void run();

  /// Stop the server.
  void stop();

  void set_accept_handler(AcceptHandler handler)
  {
	  m_acceptCallback = handler;
  }

private:
  /// Initiate an asynchronous accept operation.
  void start_accept();

  /// Handle completion of an asynchronous accept operation.
  void handle_accept(const boost::system::error_code& e);

  /// The io_service used to perform asynchronous operations.
  boost::asio::io_service io_service_;

  /// Acceptor used to listen for incoming connections.
  boost::asio::ip::tcp::acceptor acceptor_;

  /// The next connection to be accepted.
  connection_ptr new_connection_;

  boost::thread running_thread_;

  AcceptHandler m_acceptCallback;
};

