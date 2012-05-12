using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace HiFreTradeUI.Win32Invoke
{
    [StructLayout(LayoutKind.Sequential)]
    public class BreakOutStrategy
    {
        // Entry condition
        public int iRectPeriod;
        public double dRectRange;

        [MarshalAs(UnmanagedType.I1)]
        public bool bUpBreak;
        [MarshalAs(UnmanagedType.I1)]
        public bool bDownBreak;

        public double dBreakoutCriterion;
        public int iBreakoutTimespan;

        // Exit condition
        public double dGainLimit;

        // StopLoss condition
        public double dLossLimit;
    }
}
