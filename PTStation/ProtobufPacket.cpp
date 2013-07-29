#include "StdAfx.h"
#include "ProtobufPacket.h"

ProtobufPacketImpl::ProtobufPacketImpl( int serviceId, protobuf::Message* message )
{
	this->serviceId = serviceId;
	this->message = message;
	pEncodedStream = NULL;
}

ProtobufPacketImpl::~ProtobufPacketImpl( void )
{
	delete pEncodedStream;
}

bool ProtobufPacketImpl::Decode( char* pBuf, unsigned int nSize )
{
	if(nSize == 0)
		return true;
	return getStructuredData().ParseFromArray(pBuf, nSize);
}

bool ProtobufPacketImpl::Encode()
{
	if(pEncodedStream)
		return true;
	pEncodedStream = new std::string();
	if(!getStructuredData().SerializeToString(pEncodedStream))
	{
		delete pEncodedStream;
		pEncodedStream = NULL;
		return false;
	}

	getStructuredData().ByteSize();
	return true;
}

protobuf::Message& ProtobufPacketImpl::getStructuredData()
{
	return (*message);
}

int ProtobufPacketImpl::getEncodedStreamSize()
{
	return getStructuredData().GetCachedSize();
}
