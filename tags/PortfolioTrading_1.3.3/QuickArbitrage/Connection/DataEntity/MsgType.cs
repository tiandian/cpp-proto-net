using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace QuickArbitrage.Connection.DataEntity
{
    public enum MsgType
    {
        UNKNOWN,

        // System
        SYS_HEARTBEAT,
        SYS_HEARTBEAT_ACK,

        // Request
        REQ_LOGIN,

        REQ_SUBSCRIBE,
        REQ_UNSUBSCRIBE,

        // Response
        RSP_LOGIN,

        RSP_SUBSCRIBE,
        RSP_UNSUBSCRIBE,

        // Quote
        QUOTE
    }
}
