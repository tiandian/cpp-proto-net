using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel.Composition;
using System.Collections.ObjectModel;
using PortfolioTrading.Modules.Account;
using Microsoft.Practices.Prism.Commands;
using System.Windows.Input;
using System.Windows;
using Infragistics.Controls.Menus;

namespace PortfolioTrading.ViewModels
{
    [Export]
    public class AccountMgrVM
    {
        private ObservableCollection<AccountVM> _accounts = new ObservableCollection<AccountVM>();

        public IEnumerable<AccountVM> Accounts
        {
            get { return _accounts; }
        }

        public ICommand AddAccountCommand
        {
            get { return _addAccountCommand;  }
        }

        public ICommand EditAccountCommand
        {
            get { return _editAccountCommand; }
        }

        public ICommand RemoveAccountCommand
        {
            get { return _removeAccountCommand; }
        }

        private DelegateCommand _addAccountCommand;
        private DelegateCommand<XamDataTree> _editAccountCommand;
        private DelegateCommand<XamDataTree> _removeAccountCommand;
        
        public AccountMgrVM()
        {
            _addAccountCommand = new DelegateCommand(OnAddAccount);
            _editAccountCommand = new DelegateCommand<XamDataTree>(OnEditAccount);
            _removeAccountCommand = new DelegateCommand<XamDataTree>(OnRemoveAccount);
            //_accounts.Add(new AccountVM() { BrokerId="0240", InvestorId="0240050008", Password="888888" });
        }

        private void OnAddAccount()
        {
            EditAccountDlg addAcctDlg = new EditAccountDlg(true);
            addAcctDlg.Owner = Application.Current.MainWindow;
            AccountVM acct = new AccountVM();
            addAcctDlg.ViewModel = acct;
            bool? ret = addAcctDlg.ShowDialog();
            if (ret ?? false)
            {
                _accounts.Add(acct);
                //CheckCanExecute();
            }
        }

        private void OnEditAccount(XamDataTree dataTree)
        {
            AccountVM acct = GetSelectedAcccount(dataTree);
            if (acct == null) return;

            EditAccountDlg addAcctDlg = new EditAccountDlg();
            addAcctDlg.Owner = Application.Current.MainWindow;
            addAcctDlg.ViewModel = acct;
            bool? ret = addAcctDlg.ShowDialog();
        }

        private bool CanEditAccount(XamDataTree dataTree)
        {
            return GetSelectedAcccount(dataTree) != null;
        }

        private void OnRemoveAccount(XamDataTree dataTree)
        {
            AccountVM acct = GetSelectedAcccount(dataTree);
            if (acct == null) return;

            string prompt = string.Format("你确定要删除 {0} ?", acct.InvestorId);
            MessageBoxResult res = MessageBox.Show(System.Windows.Application.Current.MainWindow,
                prompt, "删除帐户", MessageBoxButton.YesNo, MessageBoxImage.Warning);
            if (res == MessageBoxResult.Yes)
            {
                this._accounts.Remove(acct);
            }
        }

        private static AccountVM GetSelectedAcccount(XamDataTree dataTree)
        {
            if (dataTree != null)
            {
                var actNode = dataTree.ActiveNode;
                if (actNode != null)
                    return actNode.Data as AccountVM;
            }
            return null;
        }

        private void CheckCanExecute()
        {
            this._editAccountCommand.RaiseCanExecuteChanged();
            this._removeAccountCommand.RaiseCanExecuteChanged();
        }

    }
}
