using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.Prism.ViewModel;
using System.ComponentModel.Composition;
using AutoTrade.MainFrame.ViewModels;
using AutoTrade.MainFrame.Modules.Account;
using AutoTrade.MainFrame.Modules.Portfolio;
using AutoTrade.MainFrame.Utils;

namespace AutoTrade.MainFrame
{
    [Export]
    class ShellViewModel : NotificationObject
    {
        public DelegateCommand AddAccountCommand { get; private set; }
        public DelegateCommand AddPortfolioCommand { get; private set; }

        private AccountManager _acctMgr;
        [Import]
        private AccountManager AccountMgr 
        {
            get { return _acctMgr; }
            set
            {
                _acctMgr = value;
                _acctMgr.OnSelectedAccountChanged += new Action<AccountInfo>(OnSelectedAccountChanged);
            }
        }

        [Import]
        private IFuturesRepositry FuturesList { get; set; }

        public ShellViewModel()
        {
            InitCommands();
        }

        public Func<System.Windows.Window> GetOwner { get; set; }

        private void InitCommands()
        {
            AddAccountCommand = new DelegateCommand(OnAddAccount);
            AddPortfolioCommand = new DelegateCommand(OnAddPortfolio, CanAddPortfolio);
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
            PortfolioItem pfItem = AccountMgr.SelectedAccount.CreatePortfolio();
            AddEditPortfolioDlg dlg = new AddEditPortfolioDlg(FuturesList, pfItem);
            bool? ret = dlg.ShowDialog();
            if (ret.HasValue && ret.Value)
            {
                AccountMgr.SelectedAccount.AddPortfolio(pfItem);
            }
        }

        public bool CanAddPortfolio(object param)
        {
            if (AccountMgr == null) return false;

            return AccountMgr.SelectedAccount != null;
        }

        void OnSelectedAccountChanged(AccountInfo obj)
        {
            AddPortfolioCommand.RaiseCanExecuteChanged();
        }
    }
}
