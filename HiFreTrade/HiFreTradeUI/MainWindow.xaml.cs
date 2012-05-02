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

            Gateway.SetSymbol("IF1205");
        }

        private void QuoteUpdateCallback(string symbol) 
        {
            Debug.WriteLine("Recevied symbol {0}", symbol);
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            this._viewModel.Connect();
        }
    }
}
