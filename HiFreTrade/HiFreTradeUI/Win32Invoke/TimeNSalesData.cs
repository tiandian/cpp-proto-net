using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace HiFreTradeUI.Win32Invoke
{
    [StructLayout(LayoutKind.Sequential)]
    public class TimeNSalesData
    {
        public const int SYMBOL_LEN = 31;
        public const int TIMESTAMP_LEN = 9;

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = SYMBOL_LEN)]
        public string caSymbol;

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = TIMESTAMP_LEN)]
        public string caTimeStamp;

        public double dPrice;
        public int iQuantity;
        public int iDirection;
    }
}
