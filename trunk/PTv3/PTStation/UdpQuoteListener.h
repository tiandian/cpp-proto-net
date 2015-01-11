#pragma once

#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/noncopyable.hpp>

using boost::asio::ip::udp;

typedef boost::function<void(char*, std::size_t)> OnDataReceivedFunc;

class CUdpQuoteListener : private boost::noncopyable
{
public:
	explicit CUdpQuoteListener(unsigned int port, OnDataReceivedFunc on_data_recv);
	~CUdpQuoteListener();

private:
	void start_receive()
	{
		recv_buffer_.fill('\0');

		socket_.async_receive_from(
			boost::asio::buffer(recv_buffer_), remote_endpoint_,
			boost::bind(&CUdpQuoteListener::handle_receive, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
	}

	void handle_receive(const boost::system::error_code& error,
		std::size_t bytes_transferred)
	{
		if (!error || error == boost::asio::error::message_size)
		{
			// handle received quote
			on_data_recv_(recv_buffer_.c_array(), bytes_transferred);

			start_receive();
		}
		else if (error == boost::asio::error::operation_aborted)
		{
			cout << "socket is closing" << endl;
		}
	}

	OnDataReceivedFunc on_data_recv_;

	/// The io_service used to perform asynchronous operations.
	boost::asio::io_service io_service_;
	boost::thread running_thread_;

	udp::socket socket_;
	udp::endpoint remote_endpoint_;
	boost::array<char, 1024> recv_buffer_;
};

typedef boost::shared_ptr<CUdpQuoteListener> UdpQuoteListenerPtr;

