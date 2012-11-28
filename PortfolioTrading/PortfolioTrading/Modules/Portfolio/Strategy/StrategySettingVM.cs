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
        private List<DirectionItem> _directionItems = new List<DirectionItem>();
        private List<CompareCondItem> _greaterCondItems = new List<CompareCondItem>();
        private List<CompareCondItem> _lessCondItems = new List<CompareCondItem>();

        public DelegateCommand ApplyCommand { get; private set; }
        public DelegateCommand ResetCommand { get; private set; }

        public StrategySettingVM()
        {
            _directionItems.Add(new DirectionItem
            {
                Direction = entity.PosiDirectionType.LONG,
                DisplayText = "多头"
            });

            _directionItems.Add(new DirectionItem
            {
                Direction = entity.PosiDirectionType.SHORT,
                DisplayText = "空头"
            });

            _greaterCondItems.Add(new CompareCondItem
            {
                Condition = CompareCondition.GREATER_EQUAL_THAN,
                DisplayText = "大于等于"
            });

            _greaterCondItems.Add(new CompareCondItem
            {
                Condition = CompareCondition.GREATER_THAN,
                DisplayText = "大于"
            });

            _lessCondItems.Add(new CompareCondItem
            {
                Condition = CompareCondition.LESS_EQUAL_THAN,
                DisplayText = "小于等于"
            });

            _lessCondItems.Add(new CompareCondItem
            {
                Condition = CompareCondition.LESS_THAN,
                DisplayText = "小于"
            });

            ApplyCommand = new DelegateCommand(OnApplySetting);
            ResetCommand = new DelegateCommand(OnResetSetting);
        }

        public IEnumerable<DirectionItem> DirectionItemsSource
        {
            get { return _directionItems; }
        }

        public IEnumerable<CompareCondItem> GreaterItemsSource
        {
            get { return _greaterCondItems; }
        }

        public IEnumerable<CompareCondItem> LessItemsSource
        {
            get { return _lessCondItems; }
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

        #region StrategyType
        private string _strategyType;

        public string StrategyType
        {
            get { return _strategyType; }
            set
            {
                if (_strategyType != value)
                {
                    _strategyType = value;
                    RaisePropertyChanged("StrategyType");
                }
            }
        }
        #endregion

        #region CurrentPortfolio
        private PortfolioVM _lastPortfVm;

        public PortfolioVM CurrentPortfolio
        {
            get { return _lastPortfVm; }    
            set
            {
                if (_lastPortfVm != value)
                {
                    _lastPortfVm = value;
                    RaisePropertyChanged("CurrentPortfolio");
                }
            }
        }
        #endregion
        

        protected virtual void OnApplySetting()
        {
            CurrentPortfolio.ApplyStrategySettings();
        }

        protected virtual void OnResetSetting()
        {
            SetPortfolio(CurrentPortfolio);
        }

        public void SetPortfolio(PortfolioVM portfVm)
        {
            this.AccountId = portfVm.AccountId;
            this.PortfolioID = portfVm.Id;
            this.StrategyType = GetStrategyDisplayName(portfVm.StrategySetting.Name);

            OnSetPortfolio(portfVm);

            this.CurrentPortfolio = portfVm;
        }

        protected virtual void OnSetPortfolio(PortfolioVM portfVm)
        { }

        private static string GetStrategyDisplayName(string name)
        {
            if (name == StrategySetting.ArbitrageStrategyName)
                return "套利";
            else if (name == StrategySetting.ChangePositionStrategyName)
                return "移仓";
            else
                return "未知";
        }
    }
}
