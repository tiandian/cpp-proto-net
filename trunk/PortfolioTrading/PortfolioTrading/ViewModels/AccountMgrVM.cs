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
using System.Xml.Linq;
using System.Reflection;
using System.IO;
using Microsoft.Practices.Prism.Events;
using PortfolioTrading.Events;

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

        private IEventAggregator EventAggregator { get; set; }

        [ImportingConstructor]
        public AccountMgrVM(IEventAggregator evtAgg)
        {
            EventAggregator = evtAgg;
            EventAggregator.GetEvent<AppShutDown>().Subscribe(OnAppShutDown);

            _addAccountCommand = new DelegateCommand(OnAddAccount);
            _editAccountCommand = new DelegateCommand<XamDataTree>(OnEditAccount);
            _removeAccountCommand = new DelegateCommand<XamDataTree>(OnRemoveAccount);
            //_accounts.Add(new AccountVM() { BrokerId="0240", InvestorId="0240050008", Password="888888" });

            Load();
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

                Persist();
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
            if (ret ?? false)
                Persist();
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
                Persist();
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

        private const string AccountsFile = "accts.xml";

        private void Persist()
        {
            XElement acctsElem = new XElement("accounts");

            foreach (var acct in _accounts)
            {
                XElement acctElem = acct.Persist();
                acctsElem.Add(acctElem);
            }
            
            acctsElem.Save(PeristFilePath);
        }

        private string PeristFilePath
        {
            get
            {
                string loc = Assembly.GetExecutingAssembly().Location;
                string dir = Path.GetDirectoryName(loc);
                return Path.Combine(dir, AccountsFile);
            }
        }

        private void Load()
        {
            string filePath = PeristFilePath;
            if (File.Exists(filePath))
            {
                XElement acctsElem = XElement.Load(filePath);
                foreach (var acctElem in acctsElem.Elements("account"))
                {
                    AccountVM acctVm = AccountVM.Load(acctElem);
                    _accounts.Add(acctVm);
                }
            }
        }

        private void OnAppShutDown(string msg)
        {
            foreach (var acct in _accounts)
            {
                if (acct.IsConnected)
                    acct.Close();
            }
        }
    }
}
