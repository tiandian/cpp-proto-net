using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.Prism.ViewModel;
using System.ComponentModel.Composition;
using AutoTrade.MainFrame.ViewModels;
using AutoTrade.MainFrame.Modules.Account;

namespace AutoTrade.MainFrame
{
    [Export]
    class ShellViewModel : NotificationObject
    {
        public DelegateCommand AddAccountCommand { get; private set; }
        public DelegateCommand AddPortfolioCommand { get; private set; }

        [Import]
        private AccountManager AccountMgr { get; set; }

        public ShellViewModel()
        {
            InitCommands();
        }

        public Func<System.Windows.Window> GetOwner { get; set; }

        private void InitCommands()
        {
            AddAccountCommand = new DelegateCommand(OnAddAccount);
        }

        public void OnAddAccount(object param)
        {
            AccountInfo acctInfo = new AccountInfo();
            AddAccountDlg dlg = new AddAccountDlg(acctInfo);
            if (GetOwner != null)
            {
                dlg.Owner = GetOwner();
            }
            bool? ret = dlg.ShowDialog();
            if (ret.HasValue && ret.Value)
            {
                AccountMgr.Add(acctInfo);
            }
        }

        public void OnAddPortfolio(object param)
        {
            
        }
    }
}
