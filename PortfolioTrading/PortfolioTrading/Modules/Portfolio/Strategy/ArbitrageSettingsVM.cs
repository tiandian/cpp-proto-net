using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel.Composition;
using Microsoft.Practices.Prism.ViewModel;
using Microsoft.Practices.Prism.Commands;
using Microsoft.Practices.Prism.Events;
using PortfolioTrading.Modules.Account;
using PortfolioTrading.Events;

namespace PortfolioTrading.Modules.Portfolio.Strategy
{
    [Export]
    public class ArbitrageSettingsVM : StrategySettingVM
    {
        public ArbitrageSettingsVM()
        {
            

        }

        public IEnumerable<CompareCondItem> OpenCondItemsSource
        {
            get;
            set;
        }

        protected override void OnApplySetting()
        {
            ArbitrageStrategySetting strategySettings = (ArbitrageStrategySetting)_lastPortfVm.StrategySetting;

            strategySettings.Direction = this.PositionDirection;
            strategySettings.OpenCondition = this.OpenCondition;
            strategySettings.OpenThreshold = this.OpenThreshold;
            strategySettings.StopGainCondition = this.StopGainCondition;
            strategySettings.StopGainThreshold = this.StopGainThreshold;
            strategySettings.StopLossCondition = this.StopLossCondition;
            strategySettings.StopLossThreshold = this.StopLossThreshold;

            base.OnApplySetting();
        }

        private bool _isInitializing = false;

        protected override void OnSetPortfolio(PortfolioVM portfVm)
        {
           
            ArbitrageStrategySetting strategySettings = (ArbitrageStrategySetting)portfVm.StrategySetting;
            _isInitializing = true;
            this.PositionDirection = strategySettings.Direction;
            this.OpenCondition = strategySettings.OpenCondition;
            this.OpenThreshold = strategySettings.OpenThreshold;
            this.StopGainCondition = strategySettings.StopGainCondition;
            this.StopGainThreshold = strategySettings.StopGainThreshold;
            this.StopLossCondition = strategySettings.StopLossCondition;
            this.StopLossThreshold = strategySettings.StopLossThreshold;
            _isInitializing = false;
        }
        
        

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

                    if (_direction == entity.PosiDirectionType.LONG)
                    {
                        OpenCondItemsSource = GreaterItemsSource;
                        RaisePropertyChanged("OpenCondItemsSource");
                        if(!_isInitializing)
                            OpenCondition = Strategy.CompareCondition.GREATER_THAN;
                    }
                    else if (_direction == entity.PosiDirectionType.SHORT)
                    {
                        OpenCondItemsSource = LessItemsSource;
                        RaisePropertyChanged("OpenCondItemsSource");
                        if (!_isInitializing)
                            OpenCondition = Strategy.CompareCondition.LESS_THAN;
                    }
                    
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
