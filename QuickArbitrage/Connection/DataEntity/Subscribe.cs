using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ProtoBuf;

namespace QuickArbitrage.Connection.DataEntity
{
    [ProtoContract]
    public class ReqSubscribe
    {
        private readonly List<string> _symbols = new List<string>();
        [ProtoMember(1)]
        public List<string> Symbols { get { return _symbols; } }

    }

    [ProtoContract]
    public class RspSubscribe
    {
        [ProtoMember(1)]
        public bool Success { get; set; }
    }

    [ProtoContract]
    public class ReqUnsubscribe
    {
        private readonly List<string> _symbols = new List<string>();
        [ProtoMember(1)]
        public List<string> Symbols { get { return _symbols; } }

    }

    [ProtoContract]
    public class RspUnsubscribe
    {
        [ProtoMember(1)]
        public bool Success { get; set; }
    }
}
