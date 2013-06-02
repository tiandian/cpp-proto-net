using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.Prism.ViewModel;
using System.Collections.ObjectModel;
using AutoTrade.MainFrame.Modules.Portfolio;

namespace AutoTrade.MainFrame.Modules.Account
{
    public class AccountInfo : NotificationObject
    {
        private ObservableCollection<Portfolio.PortfolioItem> _portfolios = new ObservableCollection<Portfolio.PortfolioItem>();

        public IEnumerable<Portfolio.PortfolioItem> Portfolios
        {
            get { return _portfolios; }
        }

        #region AccountID
        private string _accountId;

        public string AccountID
        {
            get { return _accountId; }
            set
            {
                if (_accountId != value)
                {
                    _accountId = value;
                    RaisePropertyChanged("AccountID");
                }
            }
        }
        #endregion

        #region Username
        private string _username;

        public string Username
        {
            get { return _username; }
            set
            {
                if (_username != value)
                {
                    _username = value;
                    RaisePropertyChanged("Username");
                }
            }
        }
        #endregion

        #region Password
        private string _password;

        public string Password
        {
            get { return _password; }
            set
            {
                if (_password != value)
                {
                    _password = value;
                    RaisePropertyChanged("Password");
                }
            }
        }
        #endregion


        public PortfolioItem CreatePortfolio()
        {
            return PortfolioItem.Create(AccountID, _portfolios.Count.ToString());
        }

        public void AddPortfolio(PortfolioItem item)
        {
            _portfolios.Add(item);
        }
    }
}
