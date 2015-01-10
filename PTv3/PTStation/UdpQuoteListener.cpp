#include "stdafx.h"
#include "UdpQuoteListener.h"


CUdpQuoteListener::CUdpQuoteListener(unsigned int port, OnDataReceivedFunc on_data_recv)
	: io_service_()
	, socket_(io_service_, udp::endpoint(udp::v4(), port))
	, on_data_recv_(on_data_recv)
{
	start_receive();
	running_thread_ = boost::thread(boost::bind(&boost::asio::io_service::run, &io_service_));
}

CUdpQuoteListener::~CUdpQuoteListener()
{
	socket_.close();
	running_thread_.join();
	cout << "CUdpQuoteListener destructor done." << endl;
}
