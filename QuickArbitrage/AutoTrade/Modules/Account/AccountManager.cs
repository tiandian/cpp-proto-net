using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections.ObjectModel;
using System.ComponentModel.Composition;

namespace AutoTrade.MainFrame.Modules.Account
{
    [Export]
    [PartCreationPolicy(CreationPolicy.Shared)]
    public class AccountManager
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
    }
}
