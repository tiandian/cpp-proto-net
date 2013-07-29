#include "StdAfx.h"
#include "ProtobufProtocol.h"
#include "ProtobufPacket.h"

#define SignatureStart 16018
#define SignatureEnd 16108

typedef unsigned char BYTE;

ProtobufProtocol::ProtobufProtocol(void)
{
}

ProtobufProtocol::~ProtobufProtocol()
{
    //
}

int ProtobufProtocol::encodeOutgoingPacket( OutgoingPacket& packet )
{
	ProtobufPacketImpl& gPacket = (ProtobufPacketImpl&) packet;

	return gPacket.Encode() ? PushFramework::Protocol::Success : PushFramework::Protocol::eEncodingFailure;
}

int ProtobufProtocol::frameOutgoingPacket( OutgoingPacket& packet, DataBuffer& buffer, unsigned int& nWrittenBytes )
{
	ProtobufPacketImpl& gPacket = (ProtobufPacketImpl&) packet;

	nWrittenBytes = 6 + gPacket.getEncodedStream()->size() + 2;

	if(nWrittenBytes > buffer.getRemainingSize())
		return PushFramework::Protocol::eInsufficientBuffer;
	//
	unsigned int sStart = SignatureStart;
	unsigned int packetLen = gPacket.getEncodedStreamSize() + 8;
	unsigned int commandID = gPacket.getServiceId();
	unsigned int sEnd = SignatureEnd;

	buffer.append((char*) &sStart, 2);
	buffer.append((char*)&packetLen, 2);
	buffer.append((char*)&commandID, 2);

	buffer.append((char*)gPacket.getEncodedStream()->c_str(), gPacket.getEncodedStreamSize());

	buffer.append((char*)&sEnd, 2);


	return PushFramework::Protocol::Success;
}

int ProtobufProtocol::tryDeframeIncomingPacket( DataBuffer& buffer, IncomingPacket*& pPacket, int& serviceId, unsigned int& nExtractedBytes, ConnectionContext* pContext )
{
	if(buffer.getDataSize() < 8)
		return PushFramework::Protocol::eIncompletePacket;

	char* pBuffer = buffer.getBuffer();

	unsigned int sStart = 0;
	unsigned int packetLen = 0;
	serviceId = 0;
	unsigned int sEnd = 0;

	for (int i=0;i<2;i++)
	{
		*((BYTE*)(&sStart)+i)=pBuffer[i];
		*((BYTE*)(&packetLen)+i)=pBuffer[i+2];
		*((BYTE*)(&serviceId)+i)=pBuffer[i+4];
	}

	if (sStart!=SignatureStart)
		return PushFramework::Protocol::eCorruptPacket;

	if (packetLen > buffer.getDataSize())
		return PushFramework::Protocol::eIncompletePacket;

	for (int i=0;i<2;i++)
		*((BYTE*)(&sEnd)+i)=pBuffer[packetLen-2+i];

	if(sEnd!=SignatureEnd)
		return PushFramework::Protocol::eCorruptPacket;

	char* pData = pBuffer + 6;
	unsigned int dataSize = packetLen - 8;

	nExtractedBytes = packetLen;

	ProtobufPacketImpl* pProtoBufPacket = createIncomingPacketFromServiceId(serviceId);
	if(pProtoBufPacket==NULL)
		return PushFramework::Protocol::eUndefinedFailure;

	if(!pProtoBufPacket->Decode(pData, dataSize))
	{
		disposeIncomingPacket(pProtoBufPacket);
		return PushFramework::Protocol::eDecodingFailure;
	}
	pPacket = pProtoBufPacket;
	return PushFramework::Protocol::Success;
}

int ProtobufProtocol::decodeIncomingPacket( IncomingPacket* pPacket, int& serviceId )
{
	return Protocol::Success;
}

void ProtobufProtocol::disposeOutgoingPacket( OutgoingPacket* pPacket )
{
    delete pPacket;
}

void ProtobufProtocol::disposeIncomingPacket( IncomingPacket* pPacket )
{
    delete pPacket;
}

