using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.Prism.ViewModel;
using System.ComponentModel.Composition;
using HiFreTradeUI.Utils;

namespace HiFreTradeUI.ViewModels
{
    public delegate void LoginCompletionDelegate(bool succ);

    [Export]
    public class LoginVM : NotificationObject
    {
        public LoginVM()
        {
            _accountId = RuntimeContext.TestAccountID;
        }

        #region IsUIEnabled
        private bool _isUIEnabled = true;

        public bool IsUIEnabled
        {
            get { return _isUIEnabled; }
            set
            {
                if (_isUIEnabled != value)
                {
                    _isUIEnabled = value;
                    RaisePropertyChanged("IsUIEnabled");
                }
            }
        }
        #endregion
        

        #region AccountId
        private string _accountId;

        public string AccountId
        {
            get { return _accountId; }
            set
            {
                if (_accountId != value)
                {
                    _accountId = value;
                    RaisePropertyChanged("AccountId");
                }
            }
        }
        #endregion

        public string Password { get; set; }

        private TradeVM _tradeVM;
        [Import]
        public TradeVM TradeViewModel
        {
            get { return _tradeVM; }
            set 
            { 
                _tradeVM = value;
                _tradeVM.ConnectionStatusChanged += new Action<bool>(_tradeVM_ConnectionStatusChanged);
            }
        }

        private LoginCompletionDelegate _loginCompletion;

        void _tradeVM_ConnectionStatusChanged(bool isConnected)
        {
            IsUIEnabled = true;
            if (_loginCompletion != null)
                _loginCompletion(isConnected);
        }

        public void LoginTrade(LoginCompletionDelegate loginCompletionHandler)
        {
            System.Diagnostics.Debug.Assert(!string.IsNullOrEmpty(Password), "Password cannot be empty");
            IsUIEnabled = false;

            _loginCompletion = loginCompletionHandler;
            TradeViewModel.Connect(AccountId, Password);
        }
    }
}
