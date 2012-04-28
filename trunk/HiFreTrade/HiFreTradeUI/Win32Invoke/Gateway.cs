using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace HiFreTradeUI.Win32Invoke
{
    class Gateway
    {
        [DllImport("ShFuGateway.dll", EntryPoint = "TestCall")]
        public static extern Int32 TestCall(Int32 a, Int32 b);

        [DllImport("ShFuGateway.dll")]
        public static extern bool ConnectMarketAgent(string brokerID, string userID, string password);
    }
}
