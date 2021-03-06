﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using Microsoft.Practices.Prism.ViewModel;
using PortfolioTrading.Modules.Portfolio.Strategy;
using Microsoft.Practices.Prism.Commands;
using Infragistics.Windows.Controls;

namespace PortfolioTrading.Modules.Account
{
    /// <summary>
    /// Interaction logic for EditPortfolioDlg.xaml
    /// </summary>
    public partial class EditPortfolioDlg : Window
    {
        private EditPortfolioVM _viewModel = new EditPortfolioVM();

        public EditPortfolioDlg()
        {
            InitializeComponent();
            this.DataContext = _viewModel;
        }

        private void btnOK_Click(object sender, RoutedEventArgs e)
        {
            TabItemEx tabItem = tabControl.SelectedItem as TabItemEx;
            if (tabItem != null)
                _viewModel.StrategyName = tabItem.Tag.ToString();

            _viewModel.To(Portfolio);
            this.DialogResult = true;
        }

        private void btnCancel_Click(object sender, RoutedEventArgs e)
        {
            DialogResult = false;
        }

        private PortfolioVM _portfolio;

        public PortfolioVM Portfolio
        {
            get { return _portfolio; }
            set 
            { 
                _portfolio = value;
                _viewModel.From(_portfolio);
            }
        }

        private void btnQuerySymbol_Click(object sender, RoutedEventArgs e)
        {
            if (string.IsNullOrEmpty(_viewModel.Symbol1))
            {
                MessageBox.Show("请指定合约", "缺少参数", MessageBoxButton.OK, MessageBoxImage.Error);
                return;
            }

            this.btnQuerySymbol.IsEnabled = false;

            if (Portfolio.Account.VerifyStatus())
            {
                Func<string, entity.SymbolInfo> funcQuerySymbol = Portfolio.Account.QuerySymbolInfo;
                _viewModel.IsBusy = true;
                funcQuerySymbol.BeginInvoke(_viewModel.Symbol1, new AsyncCallback(
                    delegate(IAsyncResult ar)
                    {
                        entity.SymbolInfo si = funcQuerySymbol.EndInvoke(ar);
                        if (si != null)
                        {
                            this.Dispatcher.BeginInvoke(new Action<entity.SymbolInfo>(
                                delegate(entity.SymbolInfo symbolInfo)
                                {
                                    _viewModel.PriceTick = symbolInfo.PriceTick;
                                    _viewModel.Threshold = 4 * symbolInfo.PriceTick;
                                    this.btnQuerySymbol.IsEnabled = true;
                                    _viewModel.IsBusy = false;
                                }), si);
                        }
                    }), null);
            }
        }
    }

    public class EditPortfolioVM : NotificationObject
    {
        public EditPortfolioVM()
        {
        }

        public void From(PortfolioVM portf)
        {
            PortfId = portf.Id;
            Quantity = portf.Quantity;
            MaxOpenPerStart = portf.MaxOpenPerStart;
            MaxCancelVol = portf.MaxCancel;
            TotalOpenLimit = portf.TotalOpenLimit;
            EndTimePointsExpr = portf.EndTimePointsExpr;

            int legIdx = 0;
            foreach (var leg in portf.Legs)
            {
                ++legIdx;
                if(legIdx == 1)
                {
                    Leg1 = true;
                    Symbol1 = leg.Symbol;
                    Side1 = leg.Side;
                    Ratio1 = leg.Ratio;
                }
                else
                {
                    Leg2 = true;
                    Symbol2 = leg.Symbol;
                    Side2 = leg.Side;
                    Ratio2 = leg.Ratio;
                }
            }

            if(portf.StrategySetting != null)
                StrategyName = portf.StrategySetting.Name;
        }

        public void To(PortfolioVM portf)
        {
            portf.Quantity = this.Quantity;
            portf.TotalOpenLimit = this.TotalOpenLimit;
            portf.MaxCancel = this.MaxCancelVol;
            portf.MaxOpenPerStart = this.MaxOpenPerStart;
            portf.EndTimePointsExpr = this.EndTimePointsExpr;

            if (Leg1)
            {
                LegVM leg;
                if(portf.LegCount < 1)
                {
                    leg = new LegVM();
                    portf.AddLeg(leg);
                }
                else
                {
                    leg = portf.GetLeg(0);
                }
                leg.Symbol = Symbol1;
                leg.Side = Side1;
                leg.Ratio = Ratio1;
                leg.SetIsPreferred(PreferLeg1);
            }

            if (Leg2 || StrategyName == StrategySetting.ChangePositionStrategyName)
            {
                LegVM leg;
                if (portf.LegCount < 2)
                {
                    leg = new LegVM();
                    portf.AddLeg(leg);
                }
                else
                {
                    leg = portf.GetLeg(1);
                }
                leg.Symbol = Symbol2;
                leg.Side = Side2;
                leg.Ratio = Ratio2;
                leg.IsPreferred = PreferLeg2;
            }

            portf.StrategySetting = StrategySetting.Create(StrategyName);
            if (StrategyName == StrategySetting.ArbitrageStrategyName)
            {
                ArbitrageStrategySetting setting = (ArbitrageStrategySetting)portf.StrategySetting;
                setting.BollPeriod = AR_BollPeriod;
                setting.StdDevMultiplier = AR_StdDevMultiplier;
                setting.TimeFrame = 60;
                setting.FirstLegSymbol = Symbol1;
                setting.SecondLegSymbol = Symbol2;

                foreach(var l in portf.Legs)
                {
                    l.MinPriceChange = PriceTick;
                }
            }
            else if (StrategyName == StrategySetting.ChangePositionStrategyName)
            {
                portf.AutoOpen = true;
                portf.AutoStopGain = false;
                portf.AutoStopLoss = false;
                portf.EnablePrefer = true;
                portf.AutoTracking = true;
                ChangePositionSetting setting = (ChangePositionSetting)portf.StrategySetting;
                setting.CloseLeg = Symbol1;
                setting.CloseLegSide = Side1;
            }
            else if (StrategyName == StrategySetting.ScalperStrategyName)
            {
                if(!string.IsNullOrEmpty(Symbol1))
                {
                    ScalperSetting setting = (ScalperSetting)portf.StrategySetting;
                    setting.PriceTick = this.PriceTick;
                    setting.Threshold = this.Threshold;
                }
            }
            else if (StrategyName == StrategySetting.MACDHistSlopeStrategyName)
            {
                if (!string.IsNullOrEmpty(Symbol1))
                {
                    MACDHistSlopeStrategySetting setting = (MACDHistSlopeStrategySetting)portf.StrategySetting;
                    setting.Symbol = Symbol1;
                    setting.MACD_Short = 12;
                    setting.MACD_Long = 26;
                    setting.MACD_M = 9;
                    setting.FastPeriod = this.FastPeriod;
                    setting.SlowPeriod = this.SlowPeriod;
                    setting.FastStdDiff = this.FastStdDiff;
                    setting.SlowStdDiff = this.SlowStdDiff;
                    setting.FastAngleThreshold = 45;
                    setting.SlowAngleThreshold = 30;
                    setting.TrailingStopValue = this.TrailingStop;
                }
            }
            else if (StrategyName == StrategySetting.WMATrendStrategyName)
            {
                if (!string.IsNullOrEmpty(Symbol1))
                {
                    WMATrendStrategySettings setting = (WMATrendStrategySettings)portf.StrategySetting;
                    setting.Symbol = Symbol1;
                    setting.Period = WT_Period;
                    setting.WMA_Param = WMA_Param;
                    setting.MA_N = MA_N;
                    //setting.TrailingStopValue = this.TrailingStop;
                }
            }
            else if (StrategyName == StrategySetting.LinerRegressionStrategyName)
            {
                if (!string.IsNullOrEmpty(Symbol1))
                {
                    LinerRegStrategySettings setting = (LinerRegStrategySettings)portf.StrategySetting;
                    setting.Symbol = Symbol1;
                    setting.Period = LR_Period;
                    setting.Number = LR_Number;
                    setting.OpenThreshold = LR_OpenThreshold;
                    setting.CloseThreshold = LR_CloseThreshold;
                }
            }
            else if (StrategyName == StrategySetting.ASCTrendStrategyName)
            {
                if (!string.IsNullOrEmpty(Symbol1))
                {
                    ASCTrendStrategySettings setting = (ASCTrendStrategySettings)portf.StrategySetting;
                    setting.Symbol = Symbol1;
                    setting.Period = AS_Period;
                    setting.Risk = AS_Risk;
                    setting.AvergatePeriod = AS_AveragePeriod;
                    setting.BreakoutLength = AS_BreakoutLength;
                }
            }
            else if (StrategyName == StrategySetting.RangeTrendStrategyName)
            {
                if (!string.IsNullOrEmpty(Symbol1))
                {
                    RangeTrendStrategySettings setting = (RangeTrendStrategySettings)portf.StrategySetting;
                    setting.Symbol = Symbol1;
                    setting.Period = RT_TimeFrame;
                    setting.OpenPeriod = RT_OpenPeriod;
                    setting.ClosePeriod = RT_ClosePeriod;
                    setting.StopLossFactor = RT_StopLossFactor;
                    setting.TrendFactor = RT_TrendFactor;
                }
            }
            else if (StrategyName == StrategySetting.ManualStrategyName)
            {
                if (!string.IsNullOrEmpty(Symbol1))
                {
                    ManualStrategySetting setting = (ManualStrategySetting)portf.StrategySetting;
                    setting.RetryTimes = RetryTimes;
                }
            }
        }

        #region StrategyName
        private string _strategyName = StrategySetting.ArbitrageStrategyName;

        public string StrategyName
        {
            get { return _strategyName; }
            set
            {
                if (_strategyName != value)
                {
                    _strategyName = value;
                    RaisePropertyChanged("StrategyName");
                }
            }
        }
        #endregion

        #region PortfId
        private string _portfId;

        public string PortfId
        {
            get { return _portfId; }
            set
            {
                if (_portfId != value)
                {
                    _portfId = value;
                    RaisePropertyChanged("PortfId");
                }
            }
        }
        #endregion

        #region Quantity
        private int _qty = 1;

        public int Quantity
        {
            get { return _qty; }
            set
            {
                if (_qty != value)
                {
                    _qty = value;
                    RaisePropertyChanged("Quantity");
                }
            }
        }
        #endregion

        #region TotalOpenLimit
        private int _totalOpenLimit = 450;

        public int TotalOpenLimit
        {
            get { return _totalOpenLimit; }
            set
            {
                if (_totalOpenLimit != value)
                {
                    _totalOpenLimit = value;
                    RaisePropertyChanged("TotalOpenLimit");
                }
            }
        }
        #endregion

        #region MaxCancelVol
        private int _maxCancelVol = 450;

        public int MaxCancelVol
        {
            get { return _maxCancelVol; }
            set
            {
                if (_maxCancelVol != value)
                {
                    _maxCancelVol = value;
                    RaisePropertyChanged("MaxCancelVol");
                }
            }
        }
        #endregion

        #region MaxOpenPerStart
        private int _maxOpenPerStart = 200;

        public int MaxOpenPerStart
        {
            get { return _maxOpenPerStart; }
            set
            {
                if (_maxOpenPerStart != value)
                {
                    _maxOpenPerStart = value;
                    RaisePropertyChanged("MaxOpenPerStart");
                }
            }
        }
        #endregion

        #region EndTimePointsExpr
        private string _endTimePointsExpr;

        public string EndTimePointsExpr
        {
            get { return _endTimePointsExpr; }
            set
            {
                if (_endTimePointsExpr != value)
                {
                    _endTimePointsExpr = value;
                    RaisePropertyChanged("EndTimePointsExpr");
                }
            }
        }
        #endregion

        #region Leg1
        private bool _leg1 = true;

        public bool Leg1
        {
            get { return _leg1; }
            set
            {
                if (_leg1 != value)
                {
                    _leg1 = value;
                    RaisePropertyChanged("Leg1");
                }
            }
        }
        #endregion

        #region Symbol1
        private string _symbol1;

        public string Symbol1
        {
            get { return _symbol1; }
            set
            {
                if (_symbol1 != value)
                {
                    _symbol1 = value;
                    RaisePropertyChanged("Symbol1");
                }
            }
        }
        #endregion

        #region Side1
        private PTEntity.PosiDirectionType _side1 = PTEntity.PosiDirectionType.LONG;

        public PTEntity.PosiDirectionType Side1
        {
            get { return _side1; }
            set
            {
                if (_side1 != value)
                {
                    _side1 = value;
                    RaisePropertyChanged("Side1");
                }
            }
        }
        #endregion

        #region Ratio1
        private int _ratio1 = 1;

        public int Ratio1
        {
            get { return _ratio1; }
            set
            {
                if (_ratio1 != value)
                {
                    _ratio1 = value;
                    RaisePropertyChanged("Ratio1");
                }
            }
        }
        #endregion

        #region PreferLeg1
        private bool _preferLeg1 = true;

        public bool PreferLeg1
        {
            get { return _preferLeg1; }
            set
            {
                if (_preferLeg1 != value)
                {
                    _preferLeg1 = value;
                    RaisePropertyChanged("PreferLeg1");
                }
            }
        }
        #endregion
   
        #region Leg2
        private bool _leg2;

        public bool Leg2
        {
            get { return _leg2; }
            set
            {
                if (_leg2 != value)
                {
                    _leg2 = value;
                    RaisePropertyChanged("Leg2");
                }
            }
        }
        #endregion

        #region Symbol2
        private string _symbol2;

        public string Symbol2
        {
            get { return _symbol2; }
            set
            {
                if (_symbol2 != value)
                {
                    _symbol2 = value;
                    RaisePropertyChanged("Symbol2");
                }
            }
        }
        #endregion

        #region Side2
        private PTEntity.PosiDirectionType _side2 = PTEntity.PosiDirectionType.SHORT;

        public PTEntity.PosiDirectionType Side2
        {
            get { return _side2; }
            set
            {
                if (_side2 != value)
                {
                    _side2 = value;
                    RaisePropertyChanged("Side2");
                }
            }
        }
        #endregion

        #region Ratio2
        private int _ratio2 = 1;

        public int Ratio2
        {
            get { return _ratio2; }
            set
            {
                if (_ratio2 != value)
                {
                    _ratio2 = value;
                    RaisePropertyChanged("Ratio2");
                }
            }
        }
        #endregion

        #region PreferLeg2
        private bool _preferLeg2;

        public bool PreferLeg2
        {
            get { return _preferLeg2; }
            set
            {
                if (_preferLeg2 != value)
                {
                    _preferLeg2 = value;
                    RaisePropertyChanged("PreferLeg2");
                }
            }
        }
        #endregion

        #region IsBusy
        private bool _isBusy;

        public bool IsBusy
        {
            get { return _isBusy; }
            set
            {
                if (_isBusy != value)
                {
                    _isBusy = value;
                    RaisePropertyChanged("IsBusy");
                }
            }
        }
        #endregion

        #region PriceTick
        private double _tick = 0.2;

        public double PriceTick
        {
            get { return _tick; }
            set
            {
                if (_tick != value)
                {
                    _tick = value;
                    RaisePropertyChanged("PriceTick");
                }
            }
        }
        #endregion

        #region RetryTimes
        private int _retryTimes = 8;

        public int RetryTimes
        {
            get { return _retryTimes; }
            set
            {
                if (_retryTimes != value)
                {
                    _retryTimes = value;
                    RaisePropertyChanged("RetryTimes");
                }
            }
        }
        #endregion

        #region Threshold
        private double _threshold;

        public double Threshold
        {
            get { return _threshold; }
            set
            {
                if (_threshold != value)
                {
                    _threshold = value;
                    RaisePropertyChanged("Threshold");
                }
            }
        }
        #endregion

        #region MACD Hist slope

        #region FastPeriod
        private int _fastPeriod = 60;

        public int FastPeriod
        {
            get { return _fastPeriod; }
            set
            {
                if (_fastPeriod != value)
                {
                    _fastPeriod = value;
                    RaisePropertyChanged("FastPeriod");
                }
            }
        }
        #endregion

        #region SlowPeriod
        private int _slowPeriod = 300;

        public int SlowPeriod
        {
            get { return _slowPeriod; }
            set
            {
                if (_slowPeriod != value)
                {
                    _slowPeriod = value;
                    RaisePropertyChanged("SlowPeriod");
                }
            }
        }
        #endregion

        #region FastStdDiff
        private double _fastStdDiff = 0.35;

        public double FastStdDiff
        {
            get { return _fastStdDiff; }
            set
            {
                if (_fastStdDiff != value)
                {
                    _fastStdDiff = value;
                    RaisePropertyChanged("FastStdDiff");
                }
            }
        }
        #endregion

        #region SlowStdDiff
        private double _slowStdDiff = 0.68;

        public double SlowStdDiff
        {
            get { return _slowStdDiff; }
            set
            {
                if (_slowStdDiff != value)
                {
                    _slowStdDiff = value;
                    RaisePropertyChanged("SlowStdDiff");
                }
            }
        }
        #endregion

        #region TrailingStop
        private double _trailingStop = 5;

        public double TrailingStop
        {
            get { return _trailingStop; }
            set
            {
                if (_trailingStop != value)
                {
                    _trailingStop = value;
                    RaisePropertyChanged("TrailingStop");
                }
            }
        }
        #endregion
        
        #endregion

        #region Liner Regression
        
        #region LR_Period
        private int _lr_period = 60;

        public int LR_Period
        {
            get { return _lr_period; }
            set
            {
                if (_lr_period != value)
                {
                    _lr_period = value;
                    RaisePropertyChanged("LR_Period");
                }
            }
        }
        #endregion

        #region LR_Number
        private int _lr_number = 3;

        public int LR_Number
        {
            get { return _lr_number; }
            set
            {
                if (_lr_number != value)
                {
                    _lr_number = value;
                    RaisePropertyChanged("LR_Number");
                }
            }
        }
        #endregion

        #region LR_OpenThreshold
        private double _lr_openThreshold = 25;

        public double LR_OpenThreshold
        {
            get { return _lr_openThreshold; }
            set
            {
                if (_lr_openThreshold != value)
                {
                    _lr_openThreshold = value;
                    RaisePropertyChanged("LR_OpenThreshold");
                }
            }
        }
        #endregion

        #region LR_CloseThreshold
        private double _lr_closeThreshold = 0;

        public double LR_CloseThreshold
        {
            get { return _lr_closeThreshold; }
            set
            {
                if (_lr_closeThreshold != value)
                {
                    _lr_closeThreshold = value;
                    RaisePropertyChanged("LR_CloseThreshold");
                }
            }
        }
        #endregion
        
        #endregion

        #region WMA Trend
        
        #region WT_Period
        private int _wtPeriod = 60;

        public int WT_Period
        {
            get { return _wtPeriod; }
            set
            {
                if (_wtPeriod != value)
                {
                    _wtPeriod = value;
                    RaisePropertyChanged("WT_Period");
                }
            }
        }
        #endregion

        #region WMA_Param
        private int _wmaParam = 23;

        public int WMA_Param
        {
            get { return _wmaParam; }
            set
            {
                if (_wmaParam != value)
                {
                    _wmaParam = value;
                    RaisePropertyChanged("WMA_Param");
                }
            }
        }
        #endregion

        #region MA_N
        private int _maN = 10;

        public int MA_N
        {
            get { return _maN; }
            set
            {
                if (_maN != value)
                {
                    _maN = value;
                    RaisePropertyChanged("MA_N");
                }
            }
        }
        #endregion

        #endregion

        #region ASC Trend

        #region AS_Period
        private int _as_period = 60;

        public int AS_Period
        {
            get { return _as_period; }
            set
            {
                if (_as_period != value)
                {
                    _as_period = value;
                    RaisePropertyChanged("AS_Period");
                }
            }
        }
        #endregion

        #region AS_Risk
        private int _as_risk = 5;

        public int AS_Risk
        {
            get { return _as_risk; }
            set
            {
                if (_as_risk != value)
                {
                    _as_risk = value;
                    RaisePropertyChanged("AS_Risk");
                }
            }
        }
        #endregion

        #region AS_AveragePeriod
        private int _as_averagePeriod = 10;

        public int AS_AveragePeriod
        {
            get { return _as_averagePeriod; }
            set
            {
                if (_as_averagePeriod != value)
                {
                    _as_averagePeriod = value;
                    RaisePropertyChanged("AS_AveragePeriod");
                }
            }
        }
        #endregion

        #region AS_BreakoutLength
        private int _as_breakoutLength = 2;

        public int AS_BreakoutLength
        {
            get { return _as_breakoutLength; }
            set
            {
                if (_as_breakoutLength != value)
                {
                    _as_breakoutLength = value;
                    RaisePropertyChanged("AS_BreakoutLength");
                }
            }
        }
        #endregion

        #endregion

        #region Range Trend

        #region RT_TimeFrame
        private int _rt_timeframe = 300;

        public int RT_TimeFrame
        {
            get { return _rt_timeframe; }
            set
            {
                if (_rt_timeframe != value)
                {
                    _rt_timeframe = value;
                    RaisePropertyChanged("RT_TimeFrame");
                }
            }
        }
        #endregion

        #region RT_OpenPeriod
        private int _rt_openPeriod;

        public int RT_OpenPeriod
        {
            get { return _rt_openPeriod; }
            set
            {
                if (_rt_openPeriod != value)
                {
                    _rt_openPeriod = value;
                    RaisePropertyChanged("RT_OpenPeriod");
                }
            }
        }
        #endregion

        #region RT_ClosePeriod
        private int _rt_closePeriod;

        public int RT_ClosePeriod
        {
            get { return _rt_closePeriod; }
            set
            {
                if (_rt_closePeriod != value)
                {
                    _rt_closePeriod = value;
                    RaisePropertyChanged("RT_ClosePeriod");
                }
            }
        }
        #endregion

        #region RT_StopLossFactor
        private double _rt_stopLossFactor;

        public double RT_StopLossFactor
        {
            get { return _rt_stopLossFactor; }
            set
            {
                if (_rt_stopLossFactor != value)
                {
                    _rt_stopLossFactor = value;
                    RaisePropertyChanged("RT_StopLossFactor");
                }
            }
        }
        #endregion

        #region RT_TrendFactor
        private double _rt_trendFactor;

        public double RT_TrendFactor
        {
            get { return _rt_trendFactor; }
            set
            {
                if (_rt_trendFactor != value)
                {
                    _rt_trendFactor = value;
                    RaisePropertyChanged("RT_TrendFactor");
                }
            }
        }
        #endregion
        
        #endregion

        #region Arbitrage

        #region AR_BollPeriod
        private int _ar_BollPeriod = 26;

        public int AR_BollPeriod
        {
            get { return _ar_BollPeriod; }
            set
            {
                if (_ar_BollPeriod != value)
                {
                    _ar_BollPeriod = value;
                    RaisePropertyChanged("AR_BollPeriod");
                }
            }
        }
        #endregion

        #region AR_StdDevMultiplier
        private int _ar_StdDevMultiplier = 2;

        public int AR_StdDevMultiplier
        {
            get { return _ar_StdDevMultiplier; }
            set
            {
                if (_ar_StdDevMultiplier != value)
                {
                    _ar_StdDevMultiplier = value;
                    RaisePropertyChanged("AR_StdDevMultiplier");
                }
            }
        }

        #endregion
        
        #endregion
    }

    public class StrategyItem
    {
        public string Name { get; set; }
        public string DisplayName { get; set; }
    }
}
