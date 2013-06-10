#pragma once

class ProtobufPacketImpl;

class ProtobufProtocol: public Protocol 
{
public:
    ProtobufProtocol(void);
    ~ProtobufProtocol();

    virtual void disposeOutgoingPacket(OutgoingPacket* pPacket);
    virtual void disposeIncomingPacket(IncomingPacket* pPacket);

// abstract method for inheritance to override
protected:
	virtual ProtobufPacketImpl* createIncomingPacketFromServiceId(int serviceId) = 0;

// override virtual method in Protocol
protected:
    virtual int encodeOutgoingPacket(OutgoingPacket& packet);
    virtual int frameOutgoingPacket(OutgoingPacket& packet, DataBuffer& buffer, unsigned int& nWrittenBytes);
    //
    virtual int tryDeframeIncomingPacket(DataBuffer& buffer, IncomingPacket*& pPacket, int& serviceId, unsigned int& nExtractedBytes, ConnectionContext* pContext);
    virtual int decodeIncomingPacket(IncomingPacket* pPacket, int& serviceId);
};
