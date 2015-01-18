#include "StdAfx.h"
#include "UdpQuoteListener.h"


CUdpQuoteListener::CUdpQuoteListener(unsigned int port, OnDataReceivedFunc on_data_recv)
	: io_service_()
	, socket_(io_service_)
	, on_data_recv_(on_data_recv)
{
	socket_.open(udp::v4());
	socket_.set_option(boost::asio::ip::udp::socket::reuse_address(true));
	socket_.bind(udp::endpoint(boost::asio::ip::address_v4::any(), port));

	start_receive();
	running_thread_ = boost::thread(boost::bind(&boost::asio::io_service::run, &io_service_));
}

CUdpQuoteListener::~CUdpQuoteListener()
{
	socket_.close();
	running_thread_.join();
	cout << "CUdpQuoteListener destructor done." << endl;
}
