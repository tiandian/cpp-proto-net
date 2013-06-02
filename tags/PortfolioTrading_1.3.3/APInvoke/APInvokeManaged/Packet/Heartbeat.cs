using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ProtoBuf;

namespace APInvokeManaged.Packet
{
    [ProtoContract]
    public class Heartbeat
    {
        [ProtoMember(1)]
        public int timestamp { get; set; }
    }
}
