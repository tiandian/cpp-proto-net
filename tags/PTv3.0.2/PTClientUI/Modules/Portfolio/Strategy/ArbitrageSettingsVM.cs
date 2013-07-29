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
            StopGainCondItemsSource = GreaterItemsSource;
            StopLossCondItemsSource = GreaterItemsSource;
        }

        public IEnumerable<CompareCondItem> OpenCondItemsSource
        {
            get;
            set;
        }

        public IEnumerable<CompareCondItem> StopGainCondItemsSource
        {
            get;
            set;
        }

        public IEnumerable<CompareCondItem> StopLossCondItemsSource
        {
            get;
            set;
        }

        private bool _isInitializing = false;

        protected override StrategySetting CreateSettings()
        {
            _innerSettings = new ArbitrageStrategySetting();
            _innerSettings.PropertyChanged += new System.ComponentModel.PropertyChangedEventHandler(settings_PropertyChanged);
            return _innerSettings;
        }

        private ArbitrageStrategySetting _innerSettings;

        void settings_PropertyChanged(object sender, System.ComponentModel.PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "PositionDirection")
            {
                if (_innerSettings.Direction == PTEntity.PosiDirectionType.LONG)
                {
                    OpenCondItemsSource = LessItemsSource;
                    RaisePropertyChanged("OpenCondItemsSource");

                    if (!_isInitializing)
                    {
                        _innerSettings.OpenCondition = PTEntity.CompareCondition.LESS_EQUAL_THAN;
                        CalcPossibleGain();
                        CalcPossibleLoss();
                    }
                }
                else if (_innerSettings.Direction == PTEntity.PosiDirectionType.SHORT)
                {
                    OpenCondItemsSource = GreaterItemsSource;
                    RaisePropertyChanged("OpenCondItemsSource");

                    if (!_isInitializing)
                    {
                        _innerSettings.OpenCondition = PTEntity.CompareCondition.GREATER_EQUAL_THAN;
                        CalcPossibleGain();
                        CalcPossibleLoss();
                    }
                }
            }
            else if (e.PropertyName == "OpenThreshold")
            {
                CalcPossibleGain();
                CalcPossibleLoss();
            }
            else if (e.PropertyName == "StopGainThreshold")
            {
                CalcPossibleGain();
            }
            else if (e.PropertyName == "StopLossThreshold")
            {
                CalcPossibleLoss();
            }
        }

        protected override void BeforeCopySettings()
        {
            _isInitializing = true;
            base.BeforeCopySettings();
        }

        protected override void AfterCopySettings()
        {
            _isInitializing = false;
            base.AfterCopySettings();
        }

        #region PossibleGain
        private decimal _possibleGain;

        public decimal EstimatedStopGainValue
        {
            get { return _possibleGain; }
            set
            {
                if (_possibleGain != value)
                {
                    _possibleGain = value;
                    RaisePropertyChanged("EstimatedStopGainValue");
                }
            }
        }
        #endregion
        
        #region PossibleLoss
        private decimal _possibleLoss;

        public decimal EstimatedStopLossValue
        {
            get { return _possibleLoss; }
            set
            {
                if (_possibleLoss != value)
                {
                    _possibleLoss = value;
                    RaisePropertyChanged("EstimatedStopLossValue");
                }
            }
        }
        #endregion

        private void CalcPossibleGain()
        {
            decimal stopGainVal;
            if (_innerSettings.Direction == PTEntity.PosiDirectionType.LONG)
                stopGainVal = (decimal)_innerSettings.OpenThreshold + (decimal)_innerSettings.StopGainThreshold;
            else
                stopGainVal = (decimal)_innerSettings.OpenThreshold - (decimal)_innerSettings.StopGainThreshold;
            EstimatedStopGainValue = stopGainVal;
        }

        private void CalcPossibleLoss()
        {
            decimal stopLossVal;
            if (_innerSettings.Direction == PTEntity.PosiDirectionType.LONG)
                stopLossVal = (decimal)_innerSettings.OpenThreshold - (decimal)_innerSettings.StopLossThreshold;
            else
                stopLossVal = (decimal)_innerSettings.OpenThreshold + (decimal)_innerSettings.StopLossThreshold;
            EstimatedStopLossValue = stopLossVal;
        }
    }

    public class DirectionItem
    {
        public PTEntity.PosiDirectionType Direction { get; set; }
        public string DisplayText { get; set; }
    }

    public class CompareCondItem
    {
        public PTEntity.CompareCondition Condition { get; set; }
        public string DisplayText { get; set; }
    }
}
