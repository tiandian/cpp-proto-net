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

        public delegate void AccountInfoUpdateDelegate(AccountInfoData acctInfoData);

        public delegate void PositionDetailUpdateDelegate(PositionDetail positionDetail);

        public delegate void OrderInfoUpdateDelegate(OrderInfo orderInfo);

        [DllImport("ShFuGateway.dll")]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool ConnectMarketAgent(string srvAddr, string brokerID, string userID, string password, QuoteUpdateDelegate quoteUpdateHandler);

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

        [DllImport("ShFuGateway.dll")]
        public static extern void ForceClose(string symbol, int longshort, int quantity, bool closeYesterday);

        [DllImport("ShFuGateway.dll")]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool CancelOrder();

        [DllImport("ShFuGateway.dll")]
        public static extern void SetQuantity(int quantity);

        [DllImport("ShFuGateway.dll")]
        public static extern void EnableStopGain(bool isEnabled, double gainLimit);

        [DllImport("ShFuGateway.dll")]
        public static extern void EnableStopLoss(bool isEnabled, double lossLimit);

        [DllImport("ShFuGateway.dll")]
        public static extern void RegAccountInfo(AccountInfoUpdateDelegate acctInfoCallback);

        [DllImport("ShFuGateway.dll")]
        public static extern void QueryAccountInfo();

        [DllImport("ShFuGateway.dll")]
        public static extern void RegPositionDetail(PositionDetailUpdateDelegate posiDetialCallback);

        [DllImport("ShFuGateway.dll")]
	    public static extern void QueryPositionDetail(string symbol);

        [DllImport("ShFuGateway.dll")]
        public static extern void RegOrders(OrderInfoUpdateDelegate orderCallback);

        [DllImport("ShFuGateway.dll")]
	    public static extern void QueryOrders(string symbol);

        [DllImport("ShFuGateway.dll")]
        public static extern void CancelOrder2( string ordRef,
                                                string exchId,
                                                string ordSysId,
                                                string userId,
                                                string symbol);

        [DllImport("ShFuGateway.dll")]
        public static extern int TestMarketAddress(string address);
    }
}
