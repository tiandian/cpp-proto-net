using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace HiFreTradeUI.Win32Invoke
{
    [StructLayout(LayoutKind.Sequential)]
    public class AccountInfoData
    {
        public const int BROKERID_LEN = 11;
        public const int ACCOUNTID_LEN = 13;
        public const int TRADINDG_DAY_LEN = 9;

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = BROKERID_LEN)]
        public string caBrokerId;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = ACCOUNTID_LEN)]
        public string caAccountId;

        public double dPremortgage;
        public double dPreCredit;
        public double dPreDeposit;
        public double dPreBalance;
        public double dPreMargin;
        public double dInterestBase;
        public double dInterest;
        public double dDeposit;
        public double dWithdraw;
        public double dFrozenMargin;
        public double dFrozenCash;
        public double dFrozenCommission;
        public double dCurrMargin;
        public double dCashIn;
        public double dCommission;
        public double dCloseProfit;
        public double dPositionProfit;
        public double dBalance;
        public double dAvailable;
        public double dWithdrawQuota;
        public double dReserve;

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = TRADINDG_DAY_LEN)]
        public string caTradingDay;

        public int iSettlementId;
        public double dCredit;
        public double dMortgage;
        public double dExchangeMargin;
        public double dDeliveryMargin;
        public double dExchangeDeliveryMargin;
    }
}
