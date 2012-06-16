using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ProtoBuf;

namespace APInvokeManaged.Packet
{
    [ProtoContract]
    public class Connect
    {
        [ProtoMember(1)]
        public string userid { get; set; }

        [ProtoMember(2)]
        public string password { get; set; }
    }

    [ProtoContract]
    public class ConnectAck
    {
        [ProtoMember(1)]
        public bool success { get; set; }

        [ProtoMember(2)]
        public string session { get; set; }
    }
}
