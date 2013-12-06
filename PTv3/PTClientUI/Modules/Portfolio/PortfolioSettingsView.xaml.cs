using System;
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
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.ComponentModel.Composition;
using Microsoft.Practices.Prism.Events;
using PortfolioTrading.Events;
using PortfolioTrading.Modules.Account;
using Microsoft.Practices.ServiceLocation;
using PortfolioTrading.Modules.Portfolio.Strategy;

namespace PortfolioTrading.Modules.Portfolio
{
    /// <summary>
    /// Interaction logic for PortfolioSettingsView.xaml
    /// </summary>
    [Export]
    public partial class PortfolioSettingsView : UserControl
    {


        [ImportingConstructor]
        public PortfolioSettingsView(IEventAggregator evtAgg)
        {
            InitializeComponent();
            
            evtAgg.GetEvent<PortfolioSelectedEvent>().Subscribe(OnPortfolioSelected);
        }

        private void OnPortfolioSelected(PortfolioVM portfVm)
        {
            StrategySettingVM viewModel = null;
            if(portfVm.StrategySetting.Name == StrategySetting.ArbitrageStrategyName)
            {
                viewModel = ServiceLocator.Current.GetInstance<ArbitrageSettingsVM>();
            }
            else if(portfVm.StrategySetting.Name == StrategySetting.ChangePositionStrategyName)
            {
                viewModel = ServiceLocator.Current.GetInstance<ChangePositionSettingsVM>();
            }
            else if (portfVm.StrategySetting.Name == StrategySetting.ScalperStrategyName)
            {
                viewModel = ServiceLocator.Current.GetInstance<ScalperSettingVM>();
            }
            else if (portfVm.StrategySetting.Name == StrategySetting.MACDHistSlopeStrategyName)
            {
                viewModel = ServiceLocator.Current.GetInstance<MACDHistSlopeSettingsVM>();
            }
            else if (portfVm.StrategySetting.Name == StrategySetting.WMATrendStrategyName)
            {
                viewModel = ServiceLocator.Current.GetInstance<WMATrendSettingsVM>();
            }
            else if (portfVm.StrategySetting.Name == StrategySetting.LinerRegressionStrategyName)
            {
                viewModel = ServiceLocator.Current.GetInstance<LinerRegSettingsVM>();
            }
            else if (portfVm.StrategySetting.Name == StrategySetting.ASCTrendStrategyName)
            {
                viewModel = ServiceLocator.Current.GetInstance<ASCTrendSettingsVM>();
            }

            if (viewModel != null)
            {
                viewModel.SetPortfolio(portfVm);
                this.DataContext = viewModel;
            }
        }
    }
}
