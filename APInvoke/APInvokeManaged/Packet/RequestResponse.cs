using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ProtoBuf;

namespace APInvokeManaged.Packet
{
    [ProtoContract]
    public class Request
    {
        [ProtoMember(1)]
        public string session { get; set; }

        [ProtoMember(2)]
        public string method { get; set; }

        [ProtoMember(3)]
        public byte[] param_data { get; set; }
    }

    [ProtoContract]
    public class Response
    {
        [ProtoMember(1)]
        public string method { get; set; }

        [ProtoMember(2)]
        public byte[] return_data { get; set; }

        [ProtoMember(3)]
        public bool invoke { get; set; }

        [ProtoMember(4)]
        public string error { get; set; }
    }
}
