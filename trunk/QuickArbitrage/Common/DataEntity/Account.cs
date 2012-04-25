using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ProtoBuf;

namespace QuickArbitrage.Common.DataEntity
{
    [ProtoContract]
    public class Account
    {
        [ProtoMember(1)]
        public string AccountID { get; set; }

        [ProtoMember(2)]
        public string Username { get; set; }

        [ProtoMember(3)]
        public Portfolio[] Portfolios { get; set;}
    }
}
