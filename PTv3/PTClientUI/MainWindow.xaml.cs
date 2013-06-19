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

        private const string AccountId = "0240050006";

        public MainWindow()
        {
            _host = new NativeHost();
            

            _client = new Client(new ClientHandlerImpl());
            //_client.OnError += new Action<string>(_client_OnError);
            //_client.OnQuoteReceived += new Action<entity.Quote>(_client_OnQuoteReceived);
            //_client.OnPortfolioItemUpdated += new Action<entity.PortfolioItem>(_client_OnPortfolioItemUpdated);
            //_client.OnMultiLegOrderUpdated += new Action<trade.MultiLegOrder>(_client_OnMultiLegOrderUpdated);
            //_client.OnTradeUpdated += new Action<trade.Trade>(_client_OnTradeUpdated);

            InitializeComponent();
        }

        void _client_OnTradeUpdated(trade.Trade obj)
        {
            string info = string.Format("trade: {0}\t{1}\t{2}", obj.InstrumentID, obj.Price, obj.TradeTime);
            OutputMessage(info);
        }

        void _client_OnMultiLegOrderUpdated(trade.MultiLegOrder obj)
        {
            string info = string.Format("mlOrder: {0}\t{1}\t{2}", obj.OrderId, obj.PortfolioId, obj.Quantity);
            OutputMessage(info);
        }

        void _client_OnPortfolioItemUpdated(entity.PortfolioItem obj)
        {
            string info = string.Format("Porf: {0}\t{1}\t{2}", obj.ID, obj.Quantity, null/*obj.Diff*/);
            Debug.WriteLine(info);
            OutputMessage(info);
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

            OperationResult result = _client.QuoteConnect("tcp://ctpsim-front01.gfqh.cn:43213",
                                                          AccountId);
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
            //OperationResult result = _client.QuoteLogin("1004", "00000014", "123456");
        }

        private void btnQuoteLogout_Click(object sender, RoutedEventArgs e)
        {
            _client.QuoteLogout();
        }

        private void btnTradeConn_Click(object sender, RoutedEventArgs e)
        {
            OperationResult result = _client.TradeConnect("tcp://ctpsim-front01.gfqh.cn:43205",
                                                          AccountId);
        }

        private void btnTradeClose_Click(object sender, RoutedEventArgs e)
        {
            _client.TradeDisconnect();
        }

        private void btnTradeLogin_Click(object sender, RoutedEventArgs e)
        {
            //OperationResult result = _client.TradeLogin("1004", AccountId, "123456", null);
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
            //portfolioItem.Diff = 2;
            portfolioItem.Quantity = 1;

            entity.LegItem leg1 = new entity.LegItem();
            leg1.Symbol = "cu1212";
            leg1.Side = entity.PosiDirectionType.LONG;
            leg1.Ratio = 1;
            portfolioItem.Legs.Add(leg1);

            entity.LegItem leg2 = new entity.LegItem();
            leg2.Symbol = "cu1301";
            leg2.Side = entity.PosiDirectionType.SHORT;
            leg2.Ratio = 1;
            portfolioItem.Legs.Add(leg2);

            _client.AddPortf(portfolioItem);

        }

        private void btnOpenPosi_Click(object sender, RoutedEventArgs e)
        {
            _client.PorfOpenPosition(portfolioItem.ID, 1, false);
        }

        private void btnRemovePortf_Click(object sender, RoutedEventArgs e)
        {
            _client.RemovePortf(portfolioItem.ID);
        }

        private void btnClosePosi_Click(object sender, RoutedEventArgs e)
        {
            //_client.PortfClosePosition(portfolioItem.ID, 1);
        }

        private void btnStartup_Click(object sender, RoutedEventArgs e)
        {
            //_client.ConnectAsync("127.0.0.1", 16168, (b, t, a) =>
            //{
            //    string txt = "Connect trade station ";
            //    if (b)
            //    {
            //        txt += "succeeded";
            //    }
            //    else
            //    {
            //        txt += "failed due to " + t;
            //    }
            //    OutputMessage(txt);
            //    new Action(ClientReady).BeginInvoke(null, null);
            //});
        }

        private void ClientReady()
        {
            OperationResult quoteConnResult = _client.QuoteConnect("tcp://ctpsim-front01.gfqh.cn:43213",
                                                          AccountId);
            if (quoteConnResult.Success)
            {
                OutputMessage("Quote connected");
            }
            else
            {
                OutputMessage("Connecting quote failed due to " + quoteConnResult.ErrorMessage);
                return;
            }

            OperationResult quoteLoginResult = new OperationResult(); // = _client.QuoteLogin("1004", "00000014", "123456");

            if (quoteLoginResult.Success)
            {
                OutputMessage("Quote logged in");
            }
            else
            {
                OutputMessage("Logging in quote failed due to " + quoteLoginResult.ErrorMessage);
                return;
            }

            OperationResult tradeConnResult = _client.TradeConnect("tcp://ctpsim-front01.gfqh.cn:43205",
                                                          AccountId);

            if (tradeConnResult.Success)
            {
                OutputMessage("Trade connected");
            }
            else
            {
                OutputMessage("Connecting trade failed due to " + tradeConnResult.ErrorMessage);
                return;
            }

            OperationResult tradeLoginResult = new OperationResult();// = _client.TradeLogin("1004", AccountId, "123456", null);

            if (tradeLoginResult.Success)
            {
                OutputMessage("Trade logged in");
            }
            else
            {
                OutputMessage("Logging in trade failed due to " + tradeLoginResult.ErrorMessage);
                return;
            }

        }

        private void OutputMessage(string msg)
        {
            this.Dispatcher.BeginInvoke(new Action(
                    () => this.txtOutput.AppendText(msg + "\n")));
        }
    }
}
