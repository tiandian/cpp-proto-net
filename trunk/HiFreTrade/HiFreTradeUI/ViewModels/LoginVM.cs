using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.Prism.ViewModel;

namespace HiFreTradeUI.ViewModels
{
    public class LoginVM : NotificationObject
    {
        #region IsLogingin
        private bool _isLogingin;

        public bool IsLogingin
        {
            get { return _isLogingin; }
            set
            {
                if (_isLogingin != value)
                {
                    _isLogingin = value;
                    RaisePropertyChanged("IsLogingin");
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

    }
}
