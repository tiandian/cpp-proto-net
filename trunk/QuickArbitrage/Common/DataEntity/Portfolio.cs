using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ProtoBuf;

namespace QuickArbitrage.Common.DataEntity
{
    [ProtoContract]
    public class Portfolio
    {
        [ProtoMember(1)]
        public string PID { get; set; }

        [ProtoMember(2)]
        public int Quantity { get; set; }

        [ProtoMember(3)]
        public Leg[] Legs { get; set; }
    }
}
