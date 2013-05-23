using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace APInvokeManaged
{
    public enum MsgType
    {
        UNKNOWN,

        // Connect
        CONN,

        // Connect Ack
        CONN_ACK,

        // Request
        REQ,

        // Response
        RSP,

        // Callback Req
        CALLBK_REQ,

        // Callback Rsp
        CALLBK_RSP,

        // Heartbeat
        HEARTBEAT
    }
}
