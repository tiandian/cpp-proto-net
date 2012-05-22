using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel.Composition;
using HiFreTradeUI.Win32Invoke;
using System.Diagnostics;
using Microsoft.Practices.Prism.ViewModel;
using System.Windows.Threading;

namespace HiFreTradeUI.ViewModels
{
    [Export]
    public class AccountVM : NotificationObject
    {
        private static Win32Invoke.Gateway.AccountInfoUpdateDelegate accountUpdateFunc;

        public Dispatcher UIDispatcher { get; set; }

        #region Available 可用资金
        private double _available;

        public double Available
        {
            get { return _available; }
            set
            {
                if (_available != value)
                {
                    _available = value;
                    RaisePropertyChanged("Available");
                }
            }
        }
        #endregion

        #region Balance 结算准备金
        private double _balance;

        public double Balance
        {
            get { return _balance; }
            set
            {
                if (_balance != value)
                {
                    _balance = value;
                    RaisePropertyChanged("Balance");
                }
            }
        }
        #endregion

        #region FrozenMargin 占用保证金

        private double _frozenMargin;

        public double FrozenMargin
        {
            get { return _frozenMargin; }
            set
            {
                if (_frozenMargin != value)
                {
                    _frozenMargin = value;
                    RaisePropertyChanged("FrozenMargin");
                }
            }
        }
        #endregion

        #region FrozenCash 下单冻结
        private double _frozenCash;

        public double FrozenCash
        {
            get { return _frozenCash; }
            set
            {
                if (_frozenCash != value)
                {
                    _frozenCash = value;
                    RaisePropertyChanged("FrozenCash");
                }
            }
        }
        #endregion

        #region CloseProfit 平仓盈亏
        private double _closeProfit;

        public double CloseProfit
        {
            get { return _closeProfit; }
            set
            {
                if (_closeProfit != value)
                {
                    _closeProfit = value;
                    RaisePropertyChanged("CloseProfit");
                }
            }
        }
        #endregion

        #region PositionProfit 持仓盈亏
        private double _positionProfit;

        public double PositionProfit
        {
            get { return _positionProfit; }
            set
            {
                if (_positionProfit != value)
                {
                    _positionProfit = value;
                    RaisePropertyChanged("PositionProfit");
                }
            }
        }
        #endregion

        #region CurrMargin 当前保证金
        private double _currMargin;

        public double CurrMargin
        {
            get { return _currMargin; }
            set
            {
                if (_currMargin != value)
                {
                    _currMargin = value;
                    RaisePropertyChanged("CurrMargin");
                }
            }
        }
        #endregion

        #region Commission 手续费
        private double _commission;

        public double Commission
        {
            get { return _commission; }
            set
            {
                if (_commission != value)
                {
                    _commission = value;
                    RaisePropertyChanged("Commission");
                }
            }
        }
        #endregion


        #region UpdateTime
        private DateTime _updateTime;

        public DateTime UpdateTime
        {
            get { return _updateTime; }
            set
            {
                if (_updateTime != value)
                {
                    _updateTime = value;
                    RaisePropertyChanged("UpdateTime");
                }
            }
        }
        #endregion


        public void QueryAccount()
        {
            if (accountUpdateFunc == null)
            {
                accountUpdateFunc = new Win32Invoke.Gateway.AccountInfoUpdateDelegate(OnAccountInfoUpdateDelegate);
                Win32Invoke.Gateway.RegAccountInfo(accountUpdateFunc);
            }

            Win32Invoke.Gateway.QueryAccountInfo();
        }

        private void OnAccountInfoUpdateDelegate(AccountInfoData acctInfoData)
        {
            Debug.WriteLine(acctInfoData.dAvailable);
            if (UIDispatcher != null)
            {
                UIDispatcher.BeginInvoke(new Action(delegate
                    {
                        Available = acctInfoData.dAvailable;
                        Balance = acctInfoData.dBalance;
                        CurrMargin = acctInfoData.dCurrMargin;
                        Commission = acctInfoData.dCommission;
                        FrozenMargin = acctInfoData.dFrozenMargin;
                        FrozenCash = acctInfoData.dFrozenCash;
                        CloseProfit = acctInfoData.dCloseProfit;
                        PositionProfit = acctInfoData.dPositionProfit;
                        UpdateTime = DateTime.Now;
                    }));
            }
        }
    }
}
