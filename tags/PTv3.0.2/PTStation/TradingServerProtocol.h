#pragma once

#include "ProtobufProtocol.h"

class CTradingServerProtocol : public ProtobufProtocol
{
public:
	CTradingServerProtocol(void);
	~CTradingServerProtocol(void);

	ProtobufPacketImpl* createIncomingPacketFromServiceId(int serviceId);
};

