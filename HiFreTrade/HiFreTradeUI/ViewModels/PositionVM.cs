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
    public class PositionVM : NotificationObject
    {
        private static Win32Invoke.Gateway.AccountInfoUpdateDelegate accountUpdateFunc;

        public Dispatcher UIDispatcher { get; set; }

        #region Available
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

        #region Balance
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
                        UpdateTime = DateTime.Now;
                    }));
            }
        }
    }
}
