using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel.Composition;
using Microsoft.Practices.Prism.ViewModel;
using Microsoft.Practices.Prism.Commands;
using PortfolioTrading.Modules.Portfolio.Strategy;
using Microsoft.Practices.Prism.Events;
using PortfolioTrading.Modules.Account;
using PortfolioTrading.Events;

namespace PortfolioTrading.Modules.Portfolio
{
    [Export]
    public class PortfSettingsVM : NotificationObject
    {
        private List<DirectionItem> _directionItems = new List<DirectionItem>();
        private List<CompareCondItem> _conditionItems = new List<CompareCondItem>();
        private PortfolioVM _lastPortfVm;

        [ImportingConstructor]
        public PortfSettingsVM(IEventAggregator evtAgg)
        {
            evtAgg.GetEvent<PortfolioSelectedEvent>().Subscribe(SetPortfolio);

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

            _conditionItems.Add(new CompareCondItem
            {
                Condition = CompareCondition.GREATER_EQUAL_THAN,
                DisplayText = "大于等于"
            });

            _conditionItems.Add(new CompareCondItem
            {
                Condition = CompareCondition.GREATER_THAN,
                DisplayText = "大于"
            });

            _conditionItems.Add(new CompareCondItem
            {
                Condition = CompareCondition.LESS_EQUAL_THAN,
                DisplayText = "小于等于"
            });

            _conditionItems.Add(new CompareCondItem
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

        public IEnumerable<CompareCondItem> CompareItemsSource
        {
            get { return _conditionItems; }
        }
        
        public DelegateCommand ApplyCommand { get; private set; }
        public DelegateCommand ResetCommand { get; private set; }

        private void OnApplySetting()
        {
            ArbitrageStrategySetting strategySettings = _lastPortfVm.StrategySetting as ArbitrageStrategySetting;
            strategySettings.Direction = this.PositionDirection;
            strategySettings.OpenCondition = this.OpenCondition;
            strategySettings.OpenThreshold = this.OpenThreshold;
            strategySettings.StopGainCondition = this.StopGainCondition;
            strategySettings.StopGainThreshold = this.StopGainThreshold;
            strategySettings.StopLossCondition = this.StopLossCondition;
            strategySettings.StopLossThreshold = this.StopLossThreshold;

            //_lastPortfVm.ApplyStrategySettings();
        }

        private void OnResetSetting()
        {
            SetPortfolio(_lastPortfVm);
        }

        public void SetPortfolio(PortfolioVM portfVm)
        {
            this.PortfolioID = portfVm.Id;
            if (portfVm.StrategySetting.Name == StrategySetting.ArbitrageStrategyName)
            {
                ArbitrageStrategySetting strategySettings = portfVm.StrategySetting as ArbitrageStrategySetting;
                this.PositionDirection = strategySettings.Direction;
                this.OpenCondition = strategySettings.OpenCondition;
                this.OpenThreshold = strategySettings.OpenThreshold;
                this.StopGainCondition = strategySettings.StopGainCondition;
                this.StopGainThreshold = strategySettings.StopGainThreshold;
                this.StopLossCondition = strategySettings.StopLossCondition;
                this.StopLossThreshold = strategySettings.StopLossThreshold;

                this._lastPortfVm = portfVm;
            }
            
        }

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

        #region PositionDirection
        private entity.PosiDirectionType _direction;

        public entity.PosiDirectionType PositionDirection
        {
            get { return _direction; }
            set
            {
                if (_direction != value)
                {
                    _direction = value;
                    RaisePropertyChanged("PositionDirection");
                }
            }
        }
        #endregion

        #region OpenCondition
        private CompareCondition _openCond;

        public CompareCondition OpenCondition
        {
            get { return _openCond; }
            set
            {
                if (_openCond != value)
                {
                    _openCond = value;
                    RaisePropertyChanged("OpenCondition");
                }
            }
        }
        #endregion


        #region OpenThreshold
        private double _openThreshold;

        public double OpenThreshold
        {
            get { return _openThreshold; }
            set
            {
                if (_openThreshold != value)
                {
                    _openThreshold = value;
                    RaisePropertyChanged("OpenThreshold");
                }
            }
        }
        #endregion

        #region StopGainCondition
        private CompareCondition _stopGainCond;

        public CompareCondition StopGainCondition
        {
            get { return _stopGainCond; }
            set
            {
                if (_stopGainCond != value)
                {
                    _stopGainCond = value;
                    RaisePropertyChanged("StopGainCondition");
                }
            }
        }
        #endregion


        #region StopGainThreshold
        private double _stopGainThreshold;

        public double StopGainThreshold
        {
            get { return _stopGainThreshold; }
            set
            {
                if (_stopGainThreshold != value)
                {
                    _stopGainThreshold = value;
                    RaisePropertyChanged("StopGainThreshold");
                }
            }
        }
        #endregion

        #region StopLossCondition
        private CompareCondition _stopLossCond;

        public CompareCondition StopLossCondition
        {
            get { return _stopLossCond; }
            set
            {
                if (_stopLossCond != value)
                {
                    _stopLossCond = value;
                    RaisePropertyChanged("StopLossCondition");
                }
            }
        }
        #endregion


        #region StopLossThreshold
        private double _stopLossThreshold;

        public double StopLossThreshold
        {
            get { return _stopLossThreshold; }
            set
            {
                if (_stopLossThreshold != value)
                {
                    _stopLossThreshold = value;
                    RaisePropertyChanged("StopLossThreshold");
                }
            }
        }
        #endregion

    }

    public class DirectionItem
    {
        public entity.PosiDirectionType Direction { get; set; }
        public string DisplayText { get; set; }
    }

    public class CompareCondItem
    {
        public CompareCondition Condition { get; set; }
        public string DisplayText { get; set; }
    }
}
