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
            AddAccountDlg dlg = new AddAccountDlg();
            if (GetOwner != null)
            {
                dlg.Owner = GetOwner();
            }
            dlg.ShowDialog();
        }

        public void OnAddPortfolio(object param)
        {

        }
    }
}
