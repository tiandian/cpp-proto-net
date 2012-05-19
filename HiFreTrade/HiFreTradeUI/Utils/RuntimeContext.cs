using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel.Composition;

namespace HiFreTradeUI.Utils
{
    public static class RuntimeContext
    {
        public static readonly string TestBorkerID = "0240";
        public static readonly string TestAccountID = "0240050005";
        public static readonly string TestPassword = "888888";

        public static string CurrentBrokerID 
        {
            get;
            set;
        }

        
    }
}
