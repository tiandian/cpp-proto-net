using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.Prism.ViewModel;
using Microsoft.Practices.Prism.Commands;
using PortfolioTrading.Modules.Account;

namespace PortfolioTrading.Modules.Portfolio.Strategy
{
    public abstract class StrategySettingVM : NotificationObject
    {
        private List<DirectionItem> _directionItems = new List<DirectionItem>();
        private List<CompareCondItem> _greaterCondItems = new List<CompareCondItem>();
        private List<CompareCondItem> _lessCondItems = new List<CompareCondItem>();

        public DelegateCommand ApplyCommand { get; private set; }
        public DelegateCommand ResetCommand { get; private set; }

        public StrategySettingVM()
        {
            ApplyCommand = new DelegateCommand(OnApplySetting);
            ResetCommand = new DelegateCommand(OnResetSetting);

            InitializeComboDataSource();
            this._settings = CreateSettings();
            this._settings.PropertyChanged += new System.ComponentModel.PropertyChangedEventHandler(StrategySettingVM_PropertyChanged);
        }

        void StrategySettingVM_PropertyChanged(object sender, System.ComponentModel.PropertyChangedEventArgs e)
        {
            if (!isSettingPortfolio)
            {
                IsDirty = true;
            }
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

        #region Settings
        private StrategySetting _settings;

        public StrategySetting Settings
        {
            get { return _settings; }
            set
            {
                if (_settings != value)
                {
                    _settings = value;
                    RaisePropertyChanged("Settings");
                }
            }
        }
        #endregion

        #region IsDirty
        private bool _isDirty;

        public bool IsDirty
        {
            get { return _isDirty; }
            set
            {
                if (_isDirty != value)
                {
                    _isDirty = value;
                    RaisePropertyChanged("IsDirty");
                }
            }
        }
        #endregion

        protected virtual void InitializeComboDataSource()
        {
            _directionItems.Add(new DirectionItem
            {
                Direction = PTEntity.PosiDirectionType.LONG,
                DisplayText = "多头"
            });

            _directionItems.Add(new DirectionItem
            {
                Direction = PTEntity.PosiDirectionType.SHORT,
                DisplayText = "空头"
            });

            _greaterCondItems.Add(new CompareCondItem
            {
                Condition = PTEntity.CompareCondition.GREATER_EQUAL_THAN,
                DisplayText = "大于等于"
            });

            _greaterCondItems.Add(new CompareCondItem
            {
                Condition = PTEntity.CompareCondition.GREATER_THAN,
                DisplayText = "大于"
            });

            _lessCondItems.Add(new CompareCondItem
            {
                Condition = PTEntity.CompareCondition.LESS_EQUAL_THAN,
                DisplayText = "小于等于"
            });

            _lessCondItems.Add(new CompareCondItem
            {
                Condition = PTEntity.CompareCondition.LESS_THAN,
                DisplayText = "小于"
            });
        }

        protected abstract StrategySetting CreateSettings();

        protected virtual void OnApplySetting()
        {
            CurrentPortfolio.ApplyStrategySettings(Settings);
            IsDirty = false;
        }

        protected virtual void OnResetSetting()
        {
            SetPortfolio(CurrentPortfolio);
            IsDirty = false;
        }

        private bool isSettingPortfolio = false;

        public void SetPortfolio(PortfolioVM portfVm)
        {
            this.AccountId = portfVm.AccountId;
            this.PortfolioID = portfVm.Id;
            this.StrategyType = StrategySetting.GetDisplayStrategyName(portfVm.StrategySetting.Name);
            this.CurrentPortfolio = portfVm;
            BeforeCopySettings();
            this.Settings.CopyFrom(portfVm.StrategySetting);
            AfterCopySettings();
            OnSetPortfolio(portfVm);
        }

        protected virtual void OnSetPortfolio(Account.PortfolioVM portfVm)
        {
        }

        protected virtual void BeforeCopySettings()
        {
            isSettingPortfolio = true;
        }

        protected virtual void AfterCopySettings()
        {
            isSettingPortfolio = false;
        }
    }
}
