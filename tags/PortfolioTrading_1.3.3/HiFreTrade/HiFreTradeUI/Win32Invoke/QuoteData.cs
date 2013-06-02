using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace HiFreTradeUI.Win32Invoke
{
    [StructLayout(LayoutKind.Sequential)]
    public class QuoteData
    {
        public const int SYMBOL_LEN = 31;
        public const int TRADINDG_DAY_LEN = 9;
        public const int UPDATE_TIME_LEN = 9;

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = SYMBOL_LEN)]
        public string caSymbol;

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = TRADINDG_DAY_LEN)]
        public string caTradingDay;

    	public double dLast;
	    public double dPrevClose;
	    public double dOpen;
	    public double dHigh;
	    public double dLow;
	    public int iVolume;
	    public double dTurnover;
	    public double dClose;
	
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = UPDATE_TIME_LEN)]
        public string caUpdateTime;

	    public int iUpdateMillisec;

	    public double dBid;
	    public int iBidSize;
	    public double dAsk;
	    public int iAskSize;
    }
}
