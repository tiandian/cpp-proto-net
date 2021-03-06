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
        private EditPortfolioVM _viewModel { get; set; }

        public EditPortfolioDlg()
        {
            InitializeComponent();
            
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

        public PortfolioVM Portfolio
        {
            get;
            set;
        }


        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            _viewModel = this.FindResource("editPortfolioVM") as EditPortfolioVM;
            _viewModel.From(Portfolio);
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
            MaxQuantity = portf.MaxPosition;

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
            portf.MaxPosition = this.MaxQuantity;

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
            if (StrategyName == StrategySetting.ChangePositionStrategyName)
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

        #region MaxQuantity
        private int _maxQuantity = 1;

        public int MaxQuantity
        {
            get { return _maxQuantity; }
            set
            {
                if (_maxQuantity != value)
                {
                    _maxQuantity = value;
                    RaisePropertyChanged("MaxQuantity");
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
        private entity.PosiDirectionType _side1 = entity.PosiDirectionType.SHORT;

        public entity.PosiDirectionType Side1
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
        private entity.PosiDirectionType _side2 = entity.PosiDirectionType.LONG;

        public entity.PosiDirectionType Side2
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
        private double _tick;

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
        
    }

    public class StrategyItem
    {
        public string Name { get; set; }
        public string DisplayName { get; set; }
    }
}
