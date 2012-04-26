using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections.ObjectModel;
using System.ComponentModel.Composition;
using Microsoft.Practices.Prism.ViewModel;

namespace AutoTrade.MainFrame.Modules.Account
{
    [Export]
    [PartCreationPolicy(CreationPolicy.Shared)]
    public class AccountManager : NotificationObject
    {
        private ObservableCollection<AccountInfo> _accounts = new ObservableCollection<AccountInfo>();

        public IEnumerable<AccountInfo> Accounts
        {
            get { return _accounts; }
        }

        public void Add(AccountInfo accountInfo)
        {
            _accounts.Add(accountInfo);
        }

        #region SelectedAccount
        private AccountInfo _selectedAcct;

        public AccountInfo SelectedAccount
        {
            get { return _selectedAcct; }
            set
            {
                if (_selectedAcct != value)
                {
                    _selectedAcct = value;
                    RaisePropertyChanged("SelectedAccount");
                }
            }
        }
        #endregion

        
    }
}
