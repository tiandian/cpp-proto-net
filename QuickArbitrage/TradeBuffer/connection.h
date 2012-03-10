#pragma once

#include "MsgType.h"

#include <string>
#include <sstream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>


class connection
{
public:
	/// Constructor.
	connection(boost::asio::io_service& io_service)
		: socket_(io_service)
	{
	}

	/// Get the underlying socket. Used for making a connection or for accepting
	/// an incoming connection.
	boost::asio::ip::tcp::socket& socket()
	{
		return socket_;
	}

	/// Asynchronously write a data structure to the socket.
	template <typename Handler>
	void async_write(MSG_TYPE msg_type, std::string& data, Handler handler)
	{
		// Serialize the data first so we know how large it is.
		if(data.size() == 0)
		{
			// Something went wrong, inform the caller.
			boost::system::error_code error(boost::asio::error::no_data);
			socket_.get_io_service().post(boost::bind(handler, error));
			return;
		}

		// Format the header.
		std::ostringstream header_stream;
		header_stream << std::hex 
			<< std::setw(msg_type_length) << msg_type
			 << std::setw(data_size_length) << data.size();
		if (!header_stream || header_stream.str().size() != header_length)
		{
			// Something went wrong, inform the caller.
			boost::system::error_code error(boost::asio::error::invalid_argument);
			socket_.get_io_service().post(boost::bind(handler, error));
			return;
		}
		outbound_header_ = header_stream.str();

		// Write the serialized data to the socket. We use "gather-write" to send
		// both the header and the data in a single write operation.
		std::vector<boost::asio::const_buffer> buffers;
		buffers.push_back(boost::asio::buffer(outbound_header_));
		buffers.push_back(boost::asio::buffer(data));
		boost::asio::async_write(socket_, buffers, handler);
	}

	/// Asynchronously read a data structure from the socket.
	template <typename Handler>
	void async_read(MSG_TYPE* msg_type, std::string* pData, Handler handler)
	{
		// Issue a read operation to read exactly the number of bytes in a header.
		void (connection::*f)(
			const boost::system::error_code&, MSG_TYPE*,
			std::string*, boost::tuple<Handler>)
			= &connection::handle_read_header<Handler>;

		boost::asio::async_read(socket_, boost::asio::buffer(inbound_header_),
			boost::bind(f,
			this, boost::asio::placeholders::error, msg_type, pData,
			boost::make_tuple(handler)));
	}

	/// Handle a completed read of a message header. The handler is passed using
	/// a tuple since boost::bind seems to have trouble binding a function object
	/// created using boost::bind as a parameter.
	template <typename T, typename Handler>
	void handle_read_header(const boost::system::error_code& e, MSG_TYPE* msg_type,
		std::string* pData, boost::tuple<Handler> handler)
	{
		if (e)
		{
			boost::get<0>(handler)(e);
		}
		else
		{
			// Determine the length of the serialized data.
			std::istringstream is(std::string(inbound_header_, header_length));
			is >> std::hex >> *msg_type;

			std::size_t inbound_data_size = 0;
			if (!(is >> std::hex >> inbound_data_size))
			{
				// Header doesn't seem to be valid. Inform the caller.
				boost::system::error_code error(boost::asio::error::invalid_argument);
				boost::get<0>(handler)(error);
				return;
			}

			// Start an asynchronous call to receive the data.
			inbound_data_.resize(inbound_data_size);
			void (connection::*f)(
				const boost::system::error_code&,
				std::string*, boost::tuple<Handler>)
				= &connection::handle_read_data<Handler>;
			boost::asio::async_read(socket_, boost::asio::buffer(inbound_data_),
				boost::bind(f, this,
				boost::asio::placeholders::error, pData, handler));
		}
	}

	/// Handle a completed read of message data.
	template <typename Handler>
	void handle_read_data(const boost::system::error_code& e,
		std::string* pData, boost::tuple<Handler> handler)
	{
		if (e)
		{
			boost::get<0>(handler)(e);
		}
		else
		{
			// Extract the data structure from the data just received.
			try
			{
				pData->assign(&inbound_data_[0], inbound_data_.size());
			}
			catch (std::exception& e)
			{
				// Unable to decode data.
				boost::system::error_code error(boost::asio::error::invalid_argument);
				boost::get<0>(handler)(error);
				return;
			}

			// Inform caller that data has been received ok.
			boost::get<0>(handler)(e);
		}
	}

private:
	/// The underlying socket.
	boost::asio::ip::tcp::socket socket_;

	/// The size of a fixed length header.
	enum { 
		msg_type_length = 4 ,
		data_size_length = 8,
		header_length = msg_type_length + data_size_length 
	};

	/// Holds an outbound header.
	std::string outbound_header_;

	/// Holds the outbound data.
	std::string outbound_data_;

	/// Holds an inbound header.
	char inbound_header_[header_length];

	/// Holds the inbound data.
	std::vector<char> inbound_data_;
};

typedef boost::shared_ptr<connection> connection_ptr;