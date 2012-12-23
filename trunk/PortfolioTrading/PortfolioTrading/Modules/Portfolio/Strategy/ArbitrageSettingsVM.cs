﻿using System;
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

        protected override void OnApplySetting()
        {
            ArbitrageStrategySetting strategySettings = (ArbitrageStrategySetting)CurrentPortfolio.StrategySetting;

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
                        OpenCondItemsSource = LessItemsSource;
                        StopGainCondItemsSource = GreaterItemsSource;
                        StopLossCondItemsSource = LessItemsSource;

                        RaisePropertyChanged("OpenCondItemsSource");
                        RaisePropertyChanged("StopGainCondItemsSource");
                        RaisePropertyChanged("StopLossCondItemsSource");

                        if (!_isInitializing)
                        {
                            OpenCondition = Strategy.CompareCondition.LESS_EQUAL_THAN;
                            StopGainCondition = Strategy.CompareCondition.GREATER_EQUAL_THAN;
                            StopLossCondition = Strategy.CompareCondition.LESS_EQUAL_THAN;
                            CalcPossibleGain();
                            CalcPossibleLoss();
                        }
                    }
                    else if (_direction == entity.PosiDirectionType.SHORT)
                    {
                        OpenCondItemsSource = GreaterItemsSource;
                        StopGainCondItemsSource = LessItemsSource;
                        StopLossCondItemsSource = GreaterItemsSource;

                        RaisePropertyChanged("OpenCondItemsSource");
                        RaisePropertyChanged("StopGainCondItemsSource");
                        RaisePropertyChanged("StopLossCondItemsSource");

                        if (!_isInitializing)
                        {
                            OpenCondition = Strategy.CompareCondition.GREATER_EQUAL_THAN;
                            StopGainCondition = Strategy.CompareCondition.LESS_EQUAL_THAN;
                            StopLossCondition = Strategy.CompareCondition.GREATER_EQUAL_THAN;
                            CalcPossibleGain();
                            CalcPossibleLoss();
                        }
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
                    CalcPossibleGain();
                    CalcPossibleLoss();
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
                    CalcPossibleGain();
                }
            }
        }
        #endregion

        #region PossibleGain
        private decimal _possibleGain;

        public decimal PossibleGain
        {
            get { return _possibleGain; }
            set
            {
                if (_possibleGain != value)
                {
                    _possibleGain = value;
                    RaisePropertyChanged("PossibleGain");
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
                    CalcPossibleLoss();
                }
            }
        }
        #endregion

        #region PossibleLoss
        private decimal _possibleLoss;

        public decimal PossibleLoss
        {
            get { return _possibleLoss; }
            set
            {
                if (_possibleLoss != value)
                {
                    _possibleLoss = value;
                    RaisePropertyChanged("PossibleLoss");
                }
            }
        }
        #endregion

        private void CalcPossibleGain()
        {
            decimal profit;
            if (PositionDirection == entity.PosiDirectionType.LONG)
                profit = (decimal)StopGainThreshold - (decimal)OpenThreshold;
            else
                profit = (decimal)OpenThreshold - (decimal)StopGainThreshold;
            PossibleGain = profit;
        }

        private void CalcPossibleLoss()
        {
            decimal profit;
            if (PositionDirection == entity.PosiDirectionType.LONG)
                profit = (decimal)StopLossThreshold - (decimal)OpenThreshold;
            else
                profit = (decimal)OpenThreshold - (decimal)StopLossThreshold;
            PossibleLoss = profit;
        }
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
