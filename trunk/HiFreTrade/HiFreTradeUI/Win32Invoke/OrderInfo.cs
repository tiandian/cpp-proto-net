using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace HiFreTradeUI.Win32Invoke
{
    [StructLayout(LayoutKind.Sequential)]
    public class OrderInfo
    {	
        public const int SYMBOL_LEN = 31;
        public const int ORDER_REF_LEN = 13;
        public const int INVESTORID_LEN = 13;
        public const int USERID_LEN = 16;
        public const int ORDER_SYS_ID_LEN = 21;
        public const int EXCHANGEID_LEN = 9;
        public const int TRADINDG_DAY_LEN = 9;
        public const int UPDATE_TIME_LEN = 9;
        
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = SYMBOL_LEN)]
        public string	caSymbol;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = ORDER_REF_LEN)]
	    public string	caOrderRef;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = INVESTORID_LEN)]
	    public string	caInvestorID;
	    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = USERID_LEN)]
        public string	caUserID;
	
        public char	    cHedgeFlag;
	    public int		iEntryExitType;
	    public char	    cPriceType;
	    public double	dLimitPrice;
	    public int		iVolumeTotalOriginal;
	    
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = EXCHANGEID_LEN)]
        public string	caExchangeID;
	
        public int		iOrderStatus;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = ORDER_SYS_ID_LEN)]
	    public string	caOrderSysID;
	
        public int		iVolumeTraded;
	    public int		iVolumeRemain;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = TRADINDG_DAY_LEN)]
	    public string	caInsertDate;
	    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = UPDATE_TIME_LEN)]
        public string	caInsertTime;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = UPDATE_TIME_LEN)]
	    public string	caUpdateTime;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = UPDATE_TIME_LEN)]
	    public string	caCancelTime;
        public int      iFrontID;
        public int      iSessionID;
    }
}
