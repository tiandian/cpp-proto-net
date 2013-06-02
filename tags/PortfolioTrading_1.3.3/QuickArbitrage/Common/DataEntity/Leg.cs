using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ProtoBuf;

namespace QuickArbitrage.Common.DataEntity
{
    [ProtoContract]
    public class Leg
    {
        [ProtoMember(1)]
        public string LegID { get; set; }

        [ProtoMember(2)]
        public string Code { get; set; }

        [ProtoMember(3)]
        public string Name { get; set; }

        [ProtoMember(4)]
        public int Ratio { get; set; }
    }
}
