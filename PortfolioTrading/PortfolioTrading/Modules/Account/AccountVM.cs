using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.Prism.ViewModel;
using System.Collections.ObjectModel;
using System.Windows.Input;
using Microsoft.Practices.Prism.Commands;

namespace PortfolioTrading.Modules.Account
{
    public class AccountVM : NotificationObject
    {
        private ObservableCollection<PortfolioVM> _acctPortfolios = new ObservableCollection<PortfolioVM>();

        public AccountVM()
        {
            AddPortfolioCommand = new DelegateCommand<AccountVM>(OnAddPortfolio);
        }

        public string Id
        {
            get { return string.Format("{0}-{1}", _brokerId, _investorId); }
        }

        #region BrokerId
        private string _brokerId;

        public string BrokerId
        {
            get { return _brokerId; }
            set
            {
                if (_brokerId != value)
                {
                    _brokerId = value;
                    RaisePropertyChanged("BrokerId");
                }
            }
        }
        #endregion

        #region InvestorId
        private string _investorId;

        public string InvestorId
        {
            get { return _investorId; }
            set
            {
                if (_investorId != value)
                {
                    _investorId = value;
                    RaisePropertyChanged("InvestorId");
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

        public IEnumerable<PortfolioVM> Portfolios
        {
            get { return _acctPortfolios; }
        }

        public ICommand AddPortfolioCommand
        {
            get;
            private set;
        }

        private void OnAddPortfolio(AccountVM acct)
        {
            /*
            PortfolioVM portf = new PortfolioVM();

            portf.Id = "haha";

            portf.Quantity = 1;
            portf.AddLeg(new LegVM() 
            {
                Symbol = "cu1209",
                Side = entity.PosiDirectionType.LONG,
                Ratio = 1
            });
            portf.AddLeg(new LegVM()
            {
                Symbol = "cu1210",
                Side = entity.PosiDirectionType.SHORT,
                Ratio = 1
            });
            */

            EditPortfolioDlg dlg = new EditPortfolioDlg();
            dlg.Owner = System.Windows.Application.Current.MainWindow;
            bool? res = dlg.ShowDialog();
            if (res ?? false)
            {
                PortfolioVM portf = dlg.Portfolio;
                _acctPortfolios.Add(portf);
            }
            
        }
    }
}
