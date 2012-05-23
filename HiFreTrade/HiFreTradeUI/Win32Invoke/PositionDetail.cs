using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace HiFreTradeUI.Win32Invoke
{
    [StructLayout(LayoutKind.Sequential)]
    public class PositionDetail
    {
        public const int TRADINDG_DAY_LEN = 9;
        public const int SYMBOL_LEN = 31;
        public const int EXCHANGEID_LEN = 9;
        public const int TRADEID_LEN = 21;

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = SYMBOL_LEN)]
        public string	caSymbol;

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = TRADEID_LEN)]
        public string	caTradeID;

	    public char	    cHedgeFlag;
	    public int		iDirection;
	
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = TRADINDG_DAY_LEN)]
        public string	caOpenDate;
	    public int		iVolume;
	    public double	dOpenPrice;
	
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = TRADINDG_DAY_LEN)]
        public string    caTradingDay;
	
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = EXCHANGEID_LEN)]
        public string	caExchangeID;
	    public double	dCloseProfit;
	    public double	dPositionProfit;
	    public double	dMargin;
	    public double	dMarginRateByMoney;
	    public int		CloseVolume;
	    public double	CloseAmount;
    }
}
