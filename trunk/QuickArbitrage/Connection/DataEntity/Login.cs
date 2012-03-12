using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ProtoBuf;

namespace QuickArbitrage.Connection.DataEntity
{
    [ProtoContract]
    public class ReqLogin
    {
        [ProtoMember(1)]
        public string client { get; set; }

        [ProtoMember(2)]
        public string ip { get; set; }

        [ProtoMember(3)]
        public string username { get; set; }

        [ProtoMember(4)]
        public string password { get; set; }
    }

    [ProtoContract]
    public class RspLogin
    {
        [ProtoMember(1)]
        public bool succ { get; set; }

        [ProtoMember(2)]
        public string session { get; set; }
    }
}
