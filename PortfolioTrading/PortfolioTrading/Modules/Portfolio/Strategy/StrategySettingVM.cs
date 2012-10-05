using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.Prism.ViewModel;
using Microsoft.Practices.Prism.Commands;
using PortfolioTrading.Modules.Account;

namespace PortfolioTrading.Modules.Portfolio.Strategy
{
    public class StrategySettingVM : NotificationObject
    {
        protected PortfolioVM _lastPortfVm;

        public DelegateCommand ApplyCommand { get; private set; }
        public DelegateCommand ResetCommand { get; private set; }

        public StrategySettingVM()
        {
            ApplyCommand = new DelegateCommand(OnApplySetting);
            ResetCommand = new DelegateCommand(OnResetSetting);
        }

        #region AccountId
        private string _acctId;

        public string AccountId
        {
            get { return _acctId; }
            set
            {
                if (_acctId != value)
                {
                    _acctId = value;
                    RaisePropertyChanged("AccountId");
                }
            }
        }
        #endregion

        #region PortfolioID
        private string _portfId;

        public string PortfolioID
        {
            get { return _portfId; }
            set
            {
                if (_portfId != value)
                {
                    _portfId = value;
                    RaisePropertyChanged("PortfolioID");
                }
            }
        }
        #endregion

        protected virtual void OnApplySetting()
        {
            _lastPortfVm.ApplyStrategySettings();
        }

        protected virtual void OnResetSetting()
        {
            SetPortfolio(_lastPortfVm);
        }

        public void SetPortfolio(PortfolioVM portfVm)
        {
            this.AccountId = portfVm.AccountId;
            this.PortfolioID = portfVm.Id;

            OnSetPortfolio(portfVm);

            this._lastPortfVm = portfVm;
        }

        protected virtual void OnSetPortfolio(PortfolioVM portfVm)
        { }

        
    }
}
