#pragma once

#include "ProtobufProtocol.h"

class ProtobufPacketImpl;

class CClientProtocol : public ProtobufProtocol
{
public:
	CClientProtocol(void);
	~CClientProtocol(void);

	ProtobufPacketImpl* createIncomingPacketFromServiceId(int serviceId);
};

