#pragma once

namespace PushFramework
{
class IncomingPacket;
}



struct TCPSocketEvent
{
    enum
    {
        ConnectionEstablished = 0,
        ConnectionClosed,
        NewMessage,
    };
    int type;
    int commandId;
    PushFramework::IncomingPacket* pPacket;
};
