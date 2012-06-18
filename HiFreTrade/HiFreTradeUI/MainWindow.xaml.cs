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
using HiFreTradeUI.Win32Invoke;
using System.Diagnostics;
using System.ComponentModel.Composition;
using System.Reflection;
using HiFreTradeUI.Utils;
using Microsoft.Practices.ServiceLocation;
using HiFreTradeUI.ViewModels;

namespace HiFreTradeUI
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    [Export]
    public partial class MainWindow : Window
    {
        private ShellViewModel _viewModel;

        [ImportingConstructor]
        public MainWindow(ShellViewModel viewModel, UIThread threadHolder)
        {
            LogManager.Logger.Info("MainWindow Initializing...");
            _viewModel = viewModel;

            this.DataContext = _viewModel;

            InitializeComponent();

            // Store the dispacther, so that all view models can consume
            threadHolder.Dispatcher = this.Dispatcher;
        }

        private void btnTest_Click(object sender, RoutedEventArgs e)
        {
            int ret = Gateway.TestCall(12, 32);
            Debug.WriteLine("Return value is {0}", ret);

            //bool connected = Gateway.ConnectMarketAgent("2030", "00092", "888888", QuoteUpdateCallback);
            //Debug.WriteLine("Connected {0}", connected);

            //Gateway.SetSymbol("IF1205");

            //Gateway.OpenPosition(1, 1);
        }

        private void QuoteUpdateCallback(string symbol) 
        {
            Debug.WriteLine("Recevied symbol {0}", symbol);
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            string ver = Assembly.GetExecutingAssembly().GetName().Version.ToString();
            QuoteVM quoteViewModel = ServiceLocator.Current.GetInstance<QuoteVM>();
            _viewModel.AppTitle = string.Format("High Frequency Trading v{0} [{1}] ({2})", 
                ver, RuntimeContext.CurrentUserID, quoteViewModel.MarketDataServ.Name);

            LogManager.Logger.Info("Window loaded, Connecting market...");
            this._viewModel.Connect();
        }

        private void Window_Closed(object sender, EventArgs e)
        {
            LogManager.Logger.Info("Window closed, Disconnecting market...");
            this._viewModel.Disconnect();
        }

        private void btnCancelOrder_Click(object sender, RoutedEventArgs e)
        {
            LogManager.Logger.Debug("Cancel order");
            Gateway.CancelOrder();
        }

        private void btnManualOpen_Click(object sender, RoutedEventArgs e)
        {
            int entryType = _viewModel.ManualEntryType;
            int quantity = _viewModel.Trade.OrderQty;
            Gateway.EnableStopGain(_viewModel.Trade.IsStopGainEnabled, _viewModel.Trade.GainLimit);
            Gateway.EnableStopLoss(_viewModel.Trade.IsStopLossEnabled, _viewModel.Trade.LossLimit);
            LogManager.Logger.DebugFormat("Open position. Entry:{0}, Quantity:{1}", entryType, quantity);
            Gateway.OpenPosition(quantity, entryType);
        }

        private void btnManualClose_Click(object sender, RoutedEventArgs e)
        {
            LogManager.Logger.Debug("Close position");
            Gateway.ClosePosition();
        }
    }
}
