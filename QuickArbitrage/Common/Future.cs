using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace QuickArbitrage.Common
{
    public class Future
    {
        public string Id { get; set; }

        public string Name { get; set; }

        public ExchangeID ExchangeID { get; set; }

        public string ExchName { get; set; }

        public Future()
        {
            this.Id = string.Empty;
            this.Name = string.Empty;
            this.ExchangeID = ExchangeID.Unknown;
            this.ExchName = string.Empty;
        }
    }

    public enum ExchangeID
    {
        Unknown,
        Shanghai,
        Dalian,
        Zhengzhou,
        Zhongjin
    }

    public class ExchangeInfo
    {
        public ExchangeID ID { get; set; }

        public string Name { get; set; }

        public ExchangeInfo()
        {
            this.ID = ExchangeID.Unknown;
            this.Name = string.Empty;
        }
    }
}
