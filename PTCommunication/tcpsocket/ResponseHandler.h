#pragma once

namespace PushFramework
{
class IncomingPacket;
}

class ResponseHandler
{
public:
    ResponseHandler(void)
    {
        //
    }
    ~ResponseHandler(void)
    {
        //
    }

public:
    virtual void handleResponse(PushFramework::IncomingPacket& _packet) = 0;

};
