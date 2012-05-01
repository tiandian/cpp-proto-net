using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace HiFreTradeUI.Win32Invoke
{
    [StructLayout(LayoutKind.Sequential)]
    public class OperationRecord
    {
        public const int SYMBOL_LEN = 31;
        public const int TIMEFIELD_LEN = 9;

        public int iRecordId;

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = SYMBOL_LEN)]
        public string caSymbol;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = TIMEFIELD_LEN)]
        public string caRectPeriodBegin;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = TIMEFIELD_LEN)]
        public string caRectPeriodEnd;

        public double dUpperBoundary;
        public double dLowerBoundary;
        public double dRange;
        public int iDirection;

        public double dEntryPoint;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = TIMEFIELD_LEN)]
        public string caEntryTime;

        public double dExitPoint;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = TIMEFIELD_LEN)]
        public string caExitTime;
        public int iExitReason;

        public double ProfitLoss;
    }
}
