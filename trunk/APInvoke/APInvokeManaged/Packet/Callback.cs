﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ProtoBuf;

namespace APInvokeManaged.Packet
{
    [ProtoContract]
    public class CallbackReq
    {
        [ProtoMember(1)]
        public string method { get; set; }

        [ProtoMember(2)]
        public string param_data { get; set; }
    }
    [ProtoContract]
    public class CallbackRsp
    {
        [ProtoMember(1)]
        public string session { get; set; }

        [ProtoMember(2)]
        public string method { get; set; }

        [ProtoMember(3)]
        public string return_data { get; set; }
    }
}