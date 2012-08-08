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
            _client.OnQuoteReceived += new Action<entity.Quote>(_client_OnQuoteReceived);

            InitializeComponent();
        }

        void _client_OnQuoteReceived(entity.Quote obj)
        {
            Debug.WriteLine(string.Format("{0}\t{1}\t{2}", obj.symbol, obj.last, obj.update_time));
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

        private void btnQuoteLogin_Click(object sender, RoutedEventArgs e)
        {
            OperationResult result = _client.QuoteLogin("2030", "00092", "888888");
        }

        private void btnQuoteLogout_Click(object sender, RoutedEventArgs e)
        {
            _client.QuoteLogout();
        }

        private void btnTradeConn_Click(object sender, RoutedEventArgs e)
        {
            OperationResult result = _client.TradeConnect("tcp://asp-sim2-front1.financial-trading-platform.com:26205",
                                                          "0240005010");
        }

        private void btnTradeClose_Click(object sender, RoutedEventArgs e)
        {
            _client.TradeDisconnect();
        }

        private void btnTradeLogin_Click(object sender, RoutedEventArgs e)
        {
            OperationResult result = _client.TradeLogin("0240", "0240050005", "888888");
        }

        private void btnTradeLogout_Click(object sender, RoutedEventArgs e)
        {
            _client.TradeLogout();
        }

        private void btnRegQuote_Click(object sender, RoutedEventArgs e)
        {
            string symbolContent = tbSymbols.Text.Trim();
            if (string.IsNullOrWhiteSpace(symbolContent))
                _client.RegisterQuote(new string[] { });
            else
            {
                string[] symbols = symbolContent.Split(',');
                foreach (var s in symbols)
                {
                    s.Trim();
                }
                _client.RegisterQuote(symbols);
            }
        }

        private entity.PortfolioItem portfolioItem;

        private void btnAddPortf_Click(object sender, RoutedEventArgs e)
        {
            portfolioItem = new entity.PortfolioItem();
            portfolioItem.ID = Guid.NewGuid().ToString();
            portfolioItem.AutoOpen = true;
            portfolioItem.Diff = 2;
            portfolioItem.Quantity = 1;

            entity.LegItem leg1 = new entity.LegItem();
            leg1.Symbol = "cu1208";
            leg1.Side = entity.PosiDirectionType.LONG;
            leg1.Ratio = 1;
            portfolioItem.Legs.Add(leg1);

            entity.LegItem leg2 = new entity.LegItem();
            leg2.Symbol = "cu1209";
            leg2.Side = entity.PosiDirectionType.SHORT;
            leg2.Ratio = 1;
            portfolioItem.Legs.Add(leg2);

            _client.AddPortf(portfolioItem);

        }

        private void btnOpenPosi_Click(object sender, RoutedEventArgs e)
        {
            _client.PorfOpenPosition(portfolioItem.ID, 1);
        }
    }
}
