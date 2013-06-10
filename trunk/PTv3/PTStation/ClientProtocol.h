#pragma once

class ProtobufPacketImpl;

class CClientProtocol
{
public:
	CClientProtocol(void);
	~CClientProtocol(void);

	ProtobufPacketImpl* createIncomingPacketFromServiceId(int serviceId);
};

