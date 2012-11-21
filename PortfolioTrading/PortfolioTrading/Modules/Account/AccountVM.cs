using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.Prism.ViewModel;
using System.Collections.ObjectModel;
using System.Windows.Input;
using Microsoft.Practices.Prism.Commands;
using System.Xml.Linq;
using PortfolioTrading.Infrastructure;
using System.Threading;
using PortfolioTrading.Utils;
using System.Diagnostics;
using Microsoft.Practices.Prism.Events;
using Microsoft.Practices.ServiceLocation;
using PortfolioTrading.Events;
using PortfolioTrading.Modules.Portfolio.Strategy;
using Infragistics.Windows.DataPresenter;
using System.Windows;

namespace PortfolioTrading.Modules.Account
{
    public class AccountVM : NotificationObject
    {
        private ObservableCollection<PortfolioVM> _acctPortfolios = new ObservableCollection<PortfolioVM>();

        private Client _client;
        private NativeHost _host;

        private static int HostPortSeed = 16180;

        private IEventAggregator EventAggregator { get; set; }

        public AccountVM()
        {
            AddPortfolioCommand = new DelegateCommand<AccountVM>(OnAddPortfolio);
            ConnectCommand = new DelegateCommand<AccountVM>(OnConnectHost);
            RemovePortfolioCommand = new DelegateCommand<XamDataGrid>(OnRemovePortfolio);

            _host = new NativeHost();

            EventAggregator = ServiceLocator.Current.GetInstance<IEventAggregator>();
            
            _client = new Client();
            _client.OnError += new Action<string>(_client_OnError);
            _client.OnQuoteReceived += new Action<entity.Quote>(_client_OnQuoteReceived);
            _client.OnPortfolioItemUpdated += new Action<entity.PortfolioItem>(_client_OnPortfolioItemUpdated);
            _client.OnMultiLegOrderUpdated += new Action<trade.MultiLegOrder>(_client_OnMultiLegOrderUpdated);
            _client.OnLegOrderUpdated += new Action<string, string, string, trade.Order>(_client_OnLegOrderUpdated);
            _client.OnTradeUpdated += new Action<trade.Trade>(_client_OnTradeUpdated);
            _client.OnPositionDetialReturn += new Action<trade.PositionDetailInfo>(_client_OnPositionDetialReturn);
        }

        public string Id
        {
            get { return string.Format("{0}-{1}", _brokerId, _investorId); }
        }

        #region BrokerId
        private string _brokerId;

        public string BrokerId
        {
            get { return _brokerId; }
            set
            {
                if (_brokerId != value)
                {
                    _brokerId = value;
                    RaisePropertyChanged("BrokerId");
                }
            }
        }
        #endregion

        #region InvestorId
        private string _investorId;

        public string InvestorId
        {
            get { return _investorId; }
            set
            {
                if (_investorId != value)
                {
                    _investorId = value;
                    RaisePropertyChanged("InvestorId");
                }
            }
        }
        #endregion

        #region Password
        private string _password;

        public string Password
        {
            get { return _password; }
            set
            {
                if (_password != value)
                {
                    _password = value;
                    RaisePropertyChanged("Password");
                }
            }
        }
        #endregion

        #region HostPort
        private int _hostPort;

        public int HostPort
        {
            get { return _hostPort; }
            set
            {
                if (_hostPort != value)
                {
                    _hostPort = value;
                    RaisePropertyChanged("HostPort");
                }
            }
        }
        #endregion

        #region Status
        private string _status = "未连接";

        public string Status
        {
            get { return _status; }
            set
            {
                if (_status != value)
                {
                    _status = value;
                    RaisePropertyChanged("Status");
                    RaisePropertyChanged("IsConnected");
                }
            }
        }
        #endregion


        public IEnumerable<PortfolioVM> Portfolios
        {
            get { return _acctPortfolios; }
        }

        public PortfolioVM Get(string pid)
        {
            int id = int.Parse(pid);
            return _acctPortfolios[id - 1];
        }

        public void AddPorfolio(PortfolioVM porfVm)
        {
            _acctPortfolios.Add(porfVm);
        }

        public ICommand AddPortfolioCommand
        {
            get;
            private set;
        }

        public ICommand RemovePortfolioCommand
        {
            get;
            private set;
        }

        public ICommand ConnectCommand
        {
            get;
            private set;
        }

        public void QueryAccountInfo(Action<trade.AccountInfo> accountInfoCallback)
        {
            Func<trade.AccountInfo> funcQryAcctInfo = _client.QueryAccountInfo;
            funcQryAcctInfo.BeginInvoke(ar =>
            {
                trade.AccountInfo acctInfo = funcQryAcctInfo.EndInvoke(ar);
                if (accountInfoCallback != null)
                    accountInfoCallback(acctInfo);
            }, null);
        }

        public void QueryPositionDetails(string symbol)
        {
            _client.QueryPositionDetails(symbol);
        }

        private void OnAddPortfolio(AccountVM acct)
        {
            PortfolioVM portf = new PortfolioVM(this);
            portf.Id = (_acctPortfolios.Count + 1).ToString();
            
            EditPortfolioDlg dlg = new EditPortfolioDlg();
            dlg.Owner = System.Windows.Application.Current.MainWindow;
            dlg.Portfolio = portf;
            bool? res = dlg.ShowDialog();
            if (res ?? false)
            {
                entity.PortfolioItem portfolioItem = dlg.Portfolio.GetEntity();
                _acctPortfolios.Add(portf);
                if(_client.IsConnected)
                    _client.AddPortf(portfolioItem);

                PublishChanged();
            }
        }

        private void OnRemovePortfolio(XamDataGrid dataGrid)
        {
            if (dataGrid == null) return;

            DataRecord dr = dataGrid.ActiveRecord as DataRecord;
            if (dr != null)
            {
                PortfolioVM portfVm = dr.DataItem as PortfolioVM;
                if (portfVm != null)
                {
                    MessageBoxResult res = MessageBox.Show(Application.Current.MainWindow,
                        string.Format("删除帐户[{0}]的组合 {1}", portfVm.AccountId, portfVm.DisplayText),
                        "删除确认", MessageBoxButton.YesNo, MessageBoxImage.Question);
                    if (res == MessageBoxResult.Yes)
                    {
                        _acctPortfolios.Remove(portfVm);
                        if (_client.IsConnected)
                            _client.RemovePortf(portfVm.Id);

                        PublishChanged();
                    }
                }
            }
        }

        private bool CanRemovePortfolio(XamDataGrid dataGrid)
        {
            return (dataGrid != null && dataGrid.ActiveRecord != null && dataGrid.ActiveRecord.IsDataRecord);
        }

        private void SyncToHost()
        {
            foreach (var portf in _acctPortfolios)
            {
                entity.PortfolioItem portfolioItem = portf.GetEntity();
                _client.AddPortf(portfolioItem);
            }
        }

        public bool IsConnected
        {
            get { return _status == "已连接"; }
        }

        public bool IsExpanded
        {
            get { return true; }
            set { }
        }

        public void Close()
        {
            TradeStaionCutDown();
            _client.Disconnect();
            _host.Exit();
        }

        private int _connectTimes;

        private void OnConnectHost(AccountVM acct)
        {
            Status = "连接中...";

            SynchronizationContext uiContext = SynchronizationContext.Current;

            HostPort = Interlocked.Increment(ref HostPortSeed);
            
            EventLogger.Write(string.Format("正在为{0}建立交易终端...", acct.InvestorId));

            Func<int, bool, bool> funcLaunch = new Func<int, bool, bool>(_host.Startup);
            funcLaunch.BeginInvoke(HostPort, true, 
                delegate(IAsyncResult arLaunch)
                {
                    bool succ = funcLaunch.EndInvoke(arLaunch);

                    if(!succ)
                    {
                        LogManager.Logger.Warn("Launch trade station failed");
                        return;
                    }

                    Action<int> actionLoopConnect = null;
                    _connectTimes = 1;

                    Action<bool, string> actionClntConnectDone = null;
                    actionClntConnectDone = (b, t) =>
                    {
                        string txt = string.Format("连接交易终端(第{0}次)", _connectTimes);
                        if (b)
                        {
                            txt += "成功";
                        }
                        else
                        {
                            txt += "失败 (" + t + ")";
                        }
                        EventLogger.Write(txt);

                        if (b)
                        {
                            Func<AccountVM, bool> actionReady = new Func<AccountVM, bool>(HaveTradeStationReady);
                            actionReady.BeginInvoke(
                                acct,
                                new AsyncCallback(
                                    delegate(IAsyncResult ar)
                                    {
                                        try
                                        {
                                            bool ok = actionReady.EndInvoke(ar);
                                            if (ok)
                                            {
                                                uiContext.Send(o => Status = "已连接", null);
                                                EventLogger.Write(string.Format("{0}准备就绪", acct.InvestorId));
                                                SyncToHost();
                                            }
                                            else
                                            {
                                                uiContext.Send(o => Status = "连接失败", null);
                                                EventLogger.Write(string.Format("{0}发生错误", acct.InvestorId));
                                                _host.Exit();
                                            }
                                        }
                                        catch (System.Exception ex)
                                        {
                                            EventLogger.Write("初始化交易终端发生错误");
                                            LogManager.Logger.Error(ex.Message);
                                            _host.Exit();
                                        }

                                    }),
                                null);
                            LogManager.Logger.Info(txt);
                        }
                        else
                        {
                            LogManager.Logger.Warn(txt);
                            if (_connectTimes < 10 && actionLoopConnect != null)
                                actionLoopConnect.Invoke(++_connectTimes);
                        }
                    };

                    actionLoopConnect = new Action<int>(delegate(int times)
                    {
                        Thread.Sleep(3000);
                        _client.ConnectAsync("127.0.0.1", HostPort, actionClntConnectDone);
                    });

                    actionLoopConnect.Invoke(_connectTimes);
                }, null);

            //_host.Startup(HostPort);
        }

        private bool HaveTradeStationReady(AccountVM acct)
        {
            OperationResult tradeConnResult = _client.TradeConnect("tcp://asp-sim2-front1.financial-trading-platform.com:26205",
                                                          acct.InvestorId);

            if (tradeConnResult.Success)
            {
                EventLogger.Write("交易连接成功");
            }
            else
            {
                EventLogger.Write("交易连接失败 (" + tradeConnResult.ErrorMessage + ")");
                return false;
            }

            OperationResult tradeLoginResult = _client.TradeLogin(acct.BrokerId,
                acct.InvestorId, acct.Password);

            if (tradeLoginResult.Success)
            {
                EventLogger.Write("交易登录成功");
            }
            else
            {
                EventLogger.Write("交易登录失败 (" + tradeLoginResult.ErrorMessage + ")");
                return false;
            }

            OperationResult quoteConnResult = _client.QuoteConnect("tcp://asp-sim2-md1.financial-trading-platform.com:26213",
                                                          acct.InvestorId);
            if (quoteConnResult.Success)
            {
                EventLogger.Write("行情连接成功");
            }
            else
            {
                EventLogger.Write("行情连接失败 (" + quoteConnResult.ErrorMessage + ")");
                return false;
            }

            OperationResult quoteLoginResult = _client.QuoteLogin("2030", "00092", "888888");

            if (quoteLoginResult.Success)
            {
                EventLogger.Write("行情登录成功");
            }
            else
            {
                EventLogger.Write("行情登录失败 (" + quoteLoginResult.ErrorMessage + ")");
                return false;
            }

            return true;
        }

        public void TradeStaionCutDown()
        {
            _client.QuoteLogout();
            _client.TradeLogout();
            _client.QuoteDisconnect();
            _client.TradeDisconnect();
        }

        public static AccountVM Load(XElement xmlElement)
        {
            AccountVM acct = new AccountVM();

            XAttribute attrBrokerId = xmlElement.Attribute("brokerId");
            if (attrBrokerId != null)
                acct.BrokerId = attrBrokerId.Value;

            XAttribute attrInvestorId = xmlElement.Attribute("investorId");
            if (attrInvestorId != null)
                acct.InvestorId = attrInvestorId.Value;

            XAttribute attrPwd = xmlElement.Attribute("password");
            if (attrPwd != null)
                acct.Password = attrPwd.Value;

            foreach(var portfElem in xmlElement.Element("portfolios").Elements("portfolio"))
            {
                PortfolioVM porfVm = PortfolioVM.Load(acct, portfElem);
                acct.AddPorfolio(porfVm);
            }

            return acct;
        }

        public XElement Persist()
        {
            XElement elem = new XElement("account");
            elem.Add(new XAttribute("brokerId", _brokerId));
            elem.Add(new XAttribute("investorId", _investorId));
            elem.Add(new XAttribute("password", _password));

            XElement elemPortfs = new XElement("portfolios");
            foreach (var portf in Portfolios)
            {
                elemPortfs.Add(portf.Persist());
            }
            elem.Add(elemPortfs);

            return elem;
        }

        public event Action<trade.PositionDetailInfo> OnPositionDetailReturn;

        #region Client event handlers

        void _client_OnTradeUpdated(trade.Trade obj)
        {
            string info = string.Format("trade: {0}\t{1}\t{2}", obj.InstrumentID, obj.Price, obj.TradeTime);
            EventAggregator.GetEvent<TradeUpdatedEvent>().Publish(obj);
        }

        void _client_OnMultiLegOrderUpdated(trade.MultiLegOrder obj)
        {
            string info = string.Format("mlOrder: {0}\t{1}\t{2}", obj.OrderId, obj.PortfolioId, obj.Quantity);
            EventAggregator.GetEvent<MultiLegOrderUpdatedEvent>().Publish(
                new MultiLegOrderUpdateArgs{ AccountId = Id, MultiLegOrder = obj });
        }


        void _client_OnLegOrderUpdated(string portfId, string mlOrderId, string legOrdRef, trade.Order legOrder)
        {
            OrderUpdateArgs args = new OrderUpdateArgs
                                    {
                                        AccountId = Id,
                                        PortfolioId = portfId,
                                        MlOrderId = mlOrderId,
                                        LegOrderRef = legOrdRef,
                                        LegOrder = legOrder
                                    };
            EventAggregator.GetEvent<IndividualOrderUpdatedEvent>().Publish(args);
        }

        void _client_OnPortfolioItemUpdated(entity.PortfolioItem obj)
        {
            string info = string.Format("Porf: {0}\t{1}\t{2}", obj.ID, obj.Quantity, obj.Diff);
            Debug.WriteLine(info);
            var portf = Get(obj.ID);
            portf.Update(obj);
        }

        void _client_OnQuoteReceived(entity.Quote obj)
        {
            string info = (string.Format("{0}\t{1}\t{2}", obj.symbol, obj.last, obj.update_time));
        }

        void _client_OnPositionDetialReturn(trade.PositionDetailInfo obj)
        {
            if (OnPositionDetailReturn != null)
            {
                OnPositionDetailReturn(obj);
            }
        }

        void _client_OnError(string err)
        {
            
        }
        #endregion
        
        public void PublishChanged()
        {
            EventAggregator.GetEvent<AccountChangedEvent>().Publish(this);
        }

        internal Client Host
        {
            get { return _client; }
        }
    }
}
