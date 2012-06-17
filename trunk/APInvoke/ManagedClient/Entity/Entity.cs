using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ProtoBuf;

namespace ManagedClient.Entity
{
    [ProtoContract]
    public class TestHelloParams
    {
        [ProtoMember(1)]
        public string param1 { get; set; }

        [ProtoMember(2)]
        public string param2 { get; set; }
    }

    [ProtoContract]
    public class TestHelloReturn
    {
        [ProtoMember(1)]
        public bool success { get; set; }
    }

    [ProtoContract]
    public class TestCallbackParams
    {
        [ProtoMember(1)]
        public string param3 { get; set; }
        [ProtoMember(2)]
        public string param4 { get; set; }
    }

    [ProtoContract]
    public class TestCallbackReturn
    {
        [ProtoMember(1)]
        public int RetVal { get; set; }
    }
}
