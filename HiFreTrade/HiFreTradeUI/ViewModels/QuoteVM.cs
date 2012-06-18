using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.Prism.ViewModel;
using System.ComponentModel.Composition;
using HiFreTradeUI.Win32Invoke;
using System.Diagnostics;
using HiFreTradeUI.ServerConfig;

namespace HiFreTradeUI.ViewModels
{
    [Export]
    public class QuoteVM : NotificationObject
    {
        [Import]
        private UIThread UIThread { get; set; }

        #region IsConnected
        private bool _isConnected;

        public bool IsConnected
        {
            get { return _isConnected; }
            set
            {
                if (_isConnected != value)
                {
                    _isConnected = value;
                    RaisePropertyChanged("IsConnected");
                }
            }
        }
        #endregion

        #region Last
        private double _last;

        public double Last
        {
            get { return _last; }
            set
            {
                if (_last != value)
                {
                    _last = value;
                    RaisePropertyChanged("Last");
                }
            }
        }
        #endregion

        #region Change
        private double _change;

        public double Change
        {
            get { return _change; }
            set
            {
                if (_change != value)
                {
                    _change = value;
                    RaisePropertyChanged("Change");
                }
            }
        }
        #endregion

        #region PercentChg
        private double _percentChg;

        public double PercentChg
        {
            get { return _percentChg; }
            set
            {
                if (_percentChg != value)
                {
                    _percentChg = value;
                    RaisePropertyChanged("PercentChg");
                }
            }
        }
        #endregion

        #region Ask
        private double _ask;

        public double Ask
        {
            get { return _ask; }
            set
            {
                if (_ask != value)
                {
                    _ask = value;
                    RaisePropertyChanged("Ask");
                }
            }
        }
        #endregion

        #region Bid
        private double _bid;

        public double Bid
        {
            get { return _bid; }
            set
            {
                if (_bid != value)
                {
                    _bid = value;
                    RaisePropertyChanged("Bid");
                }
            }
        }
        #endregion

        #region CurrentSymbol
        private string _currentSymbol;

        public string CurrentSymbol
        {
            get { return _currentSymbol; }
            set
            {
                if (_currentSymbol != value)
                {
                    _currentSymbol = value;
                    Gateway.SetSymbol(_currentSymbol);
                    RaisePropertyChanged("CurrentSymbol");
                }
            }
        }
        #endregion

        #region MarketDataServ
        private ServerInfo _mktServ;

        public ServerInfo MarketDataServ
        {
            get { return _mktServ; }
            set
            {
                if (_mktServ != value)
                {
                    _mktServ = value;
                    RaisePropertyChanged("MarketDataServ");
                }
            }
        }
        #endregion
        
        

        public void Connect()
        {
            Func<bool> connectFunc = new Func<bool>(ConnectGateway);
            connectFunc.BeginInvoke(
                delegate(IAsyncResult ar)
                {
                    bool connected = connectFunc.EndInvoke(ar);
                    UIThread.Dispatcher.Invoke(
                        new Action(
                            delegate
                            {
                                IsConnected = connected;
                            }));
                }, null);
        }

        public void Disconnect()
        {
            Gateway.DisconnectMarketAgent();
        }

        private static Gateway.QuoteUpdateDelegate quoteUpdateFunc;

        private bool ConnectGateway()
        {
            quoteUpdateFunc = new Gateway.QuoteUpdateDelegate(QuoteUpdateCallback);

            if (MarketDataServ.IsReal)
            {
                return Gateway.ConnectMarketAgent(MarketDataServ.Address,
                    "9000", "880810898", "9804441", quoteUpdateFunc);
            }
            else
            {
                return Gateway.ConnectMarketAgent(MarketDataServ.Address,
                    "2030", "00092", "888888", quoteUpdateFunc);
            }
        }

        private void QuoteUpdateCallback(QuoteData quote)
        {
            UIThread.Dispatcher.BeginInvoke(
                        new Action(
                            delegate
                            {
                                Last = quote.dLast;
                                Change = quote.dLast - quote.dPrevClose;
                                PercentChg = Change / quote.dPrevClose;
                                Ask = quote.dAsk == double.MaxValue ? 0d : quote.dAsk;
                                Bid = quote.dBid == double.MaxValue ? 0d : quote.dBid;
                            }));
        }

    }
}
