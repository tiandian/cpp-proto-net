#pragma once

#include "MsgType.h"

#include <string>
#include <sstream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/thread.hpp>


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

	void close()
	{
		socket_.close();
	}

	/// Asynchronously write a data structure to the socket.
	template <typename Handler>
	void async_write(MSG_TYPE msg_type, std::string& data, Handler handler)
	{
		boost::unique_lock<boost::mutex> lock(_write_mutex);

		while (!_ready_to_write)
		{
			_writable_cond.wait(lock);
		}

		_ready_to_write = false;

		void (connection::*f)(
			const boost::system::error_code&, std::size_t bytes_transferred, boost::tuple<Handler>)
			= &connection::handle_write<Handler>;

		outbound_data_.swap(data);

		// Serialize the data first so we know how large it is.
		if(outbound_data_.size() == 0)
		{
			// Something went wrong, inform the caller.
			boost::system::error_code error(boost::asio::error::no_data);
			socket_.get_io_service().post(boost::bind(f, this, error, 0, boost::make_tuple(handler)));
			return;
		}

		// Format the header.
		std::ostringstream header_stream;
		header_stream << std::hex 
			<< std::setw(msg_type_length) << msg_type
			 << std::setw(data_size_length) << outbound_data_.size();
		if (!header_stream || header_stream.str().size() != header_length)
		{
			// Something went wrong, inform the caller.
			boost::system::error_code error(boost::asio::error::invalid_argument);
			socket_.get_io_service().post(boost::bind(f, this, error, 0, boost::make_tuple(handler)));
			return;
		}
		outbound_header_ = header_stream.str();

		// Write the serialized data to the socket. We use "gather-write" to send
		// both the header and the data in a single write operation.
		std::vector<boost::asio::const_buffer> buffers;
		buffers.push_back(boost::asio::buffer(outbound_header_));
		buffers.push_back(boost::asio::buffer(outbound_data_));

		boost::asio::async_write(socket_, buffers, 
			boost::bind(f, this, _1, _2, boost::make_tuple(handler)));
	}

	/// Handle completion of a write operation.
	// NOTE that MUST NOT call async_write in write completeion handler
	template <typename Handler>
	void handle_write(const boost::system::error_code& e, std::size_t bytes_transferred, boost::tuple<Handler> handler)
	{
		boost::get<0>(handler)(e, bytes_transferred);
		{
			boost::lock_guard<boost::mutex> lock(_write_mutex);
			_ready_to_write = true;
		}
		_writable_cond.notify_one();
	}

	bool is_read_to_write()
	{
		boost::lock_guard<boost::mutex> lock(_write_mutex);
		return _ready_to_write;
	}

	/// Asynchronously read a data structure from the socket.
	template <typename Handler>
	void async_read(Handler handler)
	{
		// Issue a read operation to read exactly the number of bytes in a header.
		void (connection::*f)(
			const boost::system::error_code&, boost::tuple<Handler>)
			= &connection::handle_read_header<Handler>;

		boost::asio::async_read(socket_, boost::asio::buffer(inbound_header_),
			boost::bind(f, this, 
				boost::asio::placeholders::error, boost::make_tuple(handler)));
	}

	/// Handle a completed read of a message header. The handler is passed using
	/// a tuple since boost::bind seems to have trouble binding a function object
	/// created using boost::bind as a parameter.
	template <typename T, typename Handler>
	void handle_read_header(const boost::system::error_code& e, boost::tuple<Handler> handler)
	{
		if (e)
		{
			handle_read(e, 0, handler);
		}
		else
		{
			int nType = 0;
			// Determine the length of the serialized data.
			std::istringstream is(std::string(inbound_header_, header_length));
			is >> std::hex >> nType;
			_in_msg_type = static_cast<MSG_TYPE>(nType);

			std::size_t inbound_data_size = 0;
			if (!(is >> std::hex >> inbound_data_size))
			{
				// Header doesn't seem to be valid. Inform the caller.
				boost::system::error_code error(boost::asio::error::invalid_argument);
				handle_read(e, 0, handler);
				return;
			}

			// Start an asynchronous call to receive the data.
			inbound_data_.resize(inbound_data_size);
			void (connection::*f)(
				const boost::system::error_code&, std::size_t bytes_transferred, boost::tuple<Handler>)
				= &connection::handle_read_data<Handler>;

			boost::asio::async_read(socket_, boost::asio::buffer(inbound_data_),
				boost::bind(f, this,
					boost::asio::placeholders::error, 0, handler));
		}
	}

	/// Handle a completed read of message data.
	template <typename Handler>
	void handle_read_data(const boost::system::error_code& e, std::size_t bytes_transferred, boost::tuple<Handler> handler)
	{
		if (e)
		{
			handle_read(e, bytes_transferred, handler);
		}
		else
		{
			// Extract the data structure from the data just received.
			_in_data.assign(&inbound_data_[0], inbound_data_.size());

			handle_read(e, bytes_transferred, handler);
		}
	}

	/// Handle completion of a read operation.
	// NOTE that MUST NOT call async_read in read completeion handler
	template <typename Handler>
	void handle_read(const boost::system::error_code& e, std::size_t bytes_transferred, boost::tuple<Handler> handler)
	{
		if(e)
		{
			_in_msg_type = UNKNOWN;
			_in_data = "";
		}

		boost::get<0>(handler)(e, _in_msg_type, _in_data);
	}

	MSG_TYPE get_in_msg_type(){ return _in_msg_type; }
	std::string& get_in_data(){ return _in_data; }

private:
	/// The underlying socket.
	boost::asio::ip::tcp::socket socket_;

	/// The size of a fixed length header.
	enum { 
		msg_type_length = 4 ,
		data_size_length = 8,
		header_length = msg_type_length + data_size_length 
	};

	boost::mutex _write_mutex;
	boost::condition_variable _writable_cond;
	bool _ready_to_write;

	/// Holds an outbound header.
	std::string outbound_header_;

	/// Holds the outbound data.
	std::string outbound_data_;

	/// Holds an inbound header.
	char inbound_header_[header_length];

	/// Holds the inbound data.
	std::vector<char> inbound_data_;

	MSG_TYPE _in_msg_type;
	std::string _in_data;

};

typedef boost::shared_ptr<connection> connection_ptr;