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
using PortfolioTrading.Infrastructure;
using System.Diagnostics;

namespace PortfolioTrading
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private Client _client;
        private NativeHost _host;

        public MainWindow()
        {
            _host = new NativeHost();

            _client = new Client();
            _client.OnError += new Action<string>(_client_OnError);

            InitializeComponent();
        }

        void _client_OnError(string err)
        {
            Debug.WriteLine(err);
        }

        private void btnLaunch_Click(object sender, RoutedEventArgs e)
        {
            _host.Startup(16168);
        }

        private void btnExit_Click(object sender, RoutedEventArgs e)
        {
            _host.Exit();
        }

        private void btnConnect_Click(object sender, RoutedEventArgs e)
        {
            bool succ = _client.Connect("127.0.0.1", 16168);
            Debug.WriteLine(string.Format("Connect {0}", succ ? "Succeeded" : "Failed"));
        }

        private void btnQuoteConn_Click(object sender, RoutedEventArgs e)
        {
            
            OperationResult result = _client.QuoteConnect("tcp://asp-sim2-md1.financial-trading-platform.com:26213",
                                                          "0240005010");
            MessageBox.Show(this, string.Format("Connect: {0}. {1}", result.Success, result.ErrorMessage));
        }

        private void btnDisconnect_Click(object sender, RoutedEventArgs e)
        {

        }

        private void btnQuoteClose_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                _client.QuoteDisconnect();
            }
            catch (System.Exception ex)
            {
                MessageBox.Show(ex.Message);            	
            }
        }
    }
}
