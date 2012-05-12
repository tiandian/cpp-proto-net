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
        public static extern int TestCall(Int32 a, Int32 b);

        public delegate void QuoteUpdateDelegate(QuoteData symbol);

        public delegate void OperationRecordUpdateDelegate(OperationRecord record);

        public delegate void TimeNSalesUpdateDelegate(TimeNSalesData timeNSalesData);

        [DllImport("ShFuGateway.dll")]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool ConnectMarketAgent(string brokerID, string userID, string password, QuoteUpdateDelegate quoteUpdateHandler);

        [DllImport("ShFuGateway.dll")]
        public static extern void DisconnectMarketAgent();

        [DllImport("ShFuGateway.dll")]
        public static extern void SetSymbol(string symbol);

        [DllImport("ShFuGateway.dll")]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool ConnectTradeAgent(string brokerID, string userID, string password,
                                                    OperationRecordUpdateDelegate recordUpdateCallback,
                                                    TimeNSalesUpdateDelegate timeNSalesUpdateCallback);
        
        [DllImport("ShFuGateway.dll")]
        public static extern void DisconnectTradeAgent();

        [DllImport("ShFuGateway.dll")]
        public static extern void Start(BreakOutStrategy strategy);

        [DllImport("ShFuGateway.dll")]
        public static extern void Stop();

        [DllImport("ShFuGateway.dll")]
        public static extern void OpenPosition(int quantity, int longshort);

        [DllImport("ShFuGateway.dll")]
        public static extern void ClosePosition();
    }
}
