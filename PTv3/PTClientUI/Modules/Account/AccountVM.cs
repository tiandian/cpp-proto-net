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
        private RefDatItemsSource<PTEntity.HedgeFlagType, string> _hedgeFlagItemsSource = new RefDatItemsSource<PTEntity.HedgeFlagType,string>();

        private Client _client;
        private ClientHandlerImpl _clientHandler;
        private NativeHost _host;

        private static int HostPortSeed = 16181;
        private static int MaxRetryConnectTimes = 3;

        private IEventAggregator EventAggregator { get; set; }
        private ServerAddressRepoVM AddressRepo { get; set; }

        private SynchronizationContext UIContext { get; set; }

        public AccountVM()
        {
            AddPortfolioCommand = new DelegateCommand<AccountVM>(OnAddPortfolio);
            ConnectCommand = new DelegateCommand<AccountVM>(OnConnectHost);
            DisconnectCommand = new DelegateCommand<AccountVM>(OnDisconnectHost);
            RemovePortfolioCommand = new DelegateCommand<XamDataGrid>(OnRemovePortfolio);
            DetachCommand = new DelegateCommand<AccountVM>(OnDetachHost);
            StartAllPortfolioCmd = new DelegateCommand<AccountVM>(OnStartAllPortfolio);
            StopAllPortfolioCmd = new DelegateCommand<AccountVM>(OnStopAllPortfolio);

            _host = new NativeHost();
            AccountInfo = new AccountInfoVM(this);

            EventAggregator = ServiceLocator.Current.GetInstance<IEventAggregator>();
            AddressRepo = ServiceLocator.Current.GetInstance<ServerAddressRepoVM>();

            _clientHandler = new ClientHandlerImpl();
            _client = new Client(_clientHandler);
            _clientHandler.OnPortfolioUpdated += new Action<PTEntity.PortfolioUpdateItem>(_clientHandler_OnPortfolioUpdated);
            _clientHandler.OnMultiLegOrderUpdated += new Action<PTEntity.MultiLegOrder>(_client_OnMultiLegOrderUpdated);
            _clientHandler.OnLegOrderUpdated += new Action<string, string, string, PTEntity.Order>(_client_OnLegOrderUpdated);
            _clientHandler.OnTradeUpdated += new Action<PTEntity.TradeUpdate>(_client_OnTradeUpdated);
            _clientHandler.OnConnectionClosed += new Action(_clientHandler_OnConnectionClosed);
            /*
            _client.OnError += new Action<string>(_client_OnError);
            _client.OnQuoteReceived += new Action<entity.Quote>(_client_OnQuoteReceived);
            _client.OnPositionDetialReturn += new Action<trade.PositionDetailInfo>(_client_OnPositionDetialReturn);
            */

            _hedgeFlagItemsSource.Add(new RefDataItem<PTEntity.HedgeFlagType, string> 
                { Value = PTEntity.HedgeFlagType.SPECULATION, DisplayText = "投机" });
            _hedgeFlagItemsSource.Add(new RefDataItem<PTEntity.HedgeFlagType, string> 
                { Value = PTEntity.HedgeFlagType.ARBITRAGE, DisplayText = "套利" });
            _hedgeFlagItemsSource.Add(new RefDataItem<PTEntity.HedgeFlagType, string> 
                { Value = PTEntity.HedgeFlagType.HEDGE, DisplayText = "套保" });
        }

        void _clientHandler_OnConnectionClosed()
        {
            _client.EndHeartbeat();

            UIContext.Post(
                o => { 
                    ChangeStatus("未连接", false);
                    EventLogger.Write("与交易终端的连接丢失");
                }, 
                null);
        }

        #region Properties

        public AccountInfoVM AccountInfo
        {
            get;
            private set;
        }

        public string Id
        {
            get { return string.Format("{0}-{1}", _brokerId, _investorId); }
        }

        #region BrokerId
        private string _brokerId = string.Empty;

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
        private string _investorId = string.Empty;

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

        #region UserId
        private string _userId = string.Empty;

        public string UserId
        {
            get { return _userId; }
            set
            {
                if (_userId != value)
                {
                    _userId = value;
                    RaisePropertyChanged("UserId");
                }
            }
        }
        #endregion
        

        #region Password
        private string _password = string.Empty;

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

        #region MaxSubmit
        private int _maxSubmit = 450;

        public int MaxSubmit
        {
            get { return _maxSubmit; }
            set
            {
                if (_maxSubmit != value)
                {
                    _maxSubmit = value;
                    RaisePropertyChanged("MaxSubmit");
                }
            }
        }
        #endregion

        #region MaxCancel
        private int _maxCancel = 900;

        public int MaxCancel
        {
            get { return _maxCancel; }
            set
            {
                if (_maxCancel != value)
                {
                    _maxCancel = value;
                    RaisePropertyChanged("MaxCancel");
                }
            }
        }
        #endregion

        #region HedgeFlag
        private PTEntity.HedgeFlagType _hedgeFlag = PTEntity.HedgeFlagType.SPECULATION;

        public PTEntity.HedgeFlagType HedgeFlag
        {
            get { return _hedgeFlag; }
            set
            {
                if (_hedgeFlag != value)
                {
                    _hedgeFlag = value;
                    RaisePropertyChanged("HedgeFlag");
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

        #endregion

        public RefDatItemsSource<PTEntity.HedgeFlagType, string> HedgeFlagItemsSource
        {
            get { return _hedgeFlagItemsSource; }
        }

        public IEnumerable<PortfolioVM> Portfolios
        {
            get { return _acctPortfolios; }
        }

        public PortfolioVM Get(string pid)
        {
            lock (_acctPortfolios)
            {
                return _acctPortfolios.FirstOrDefault(p => p.Id == pid);
            }
        }

        public void AddPorfolio(PortfolioVM porfVm)
        {
            lock (_acctPortfolios)
            {
                _acctPortfolios.Add(porfVm);
            }
        }

        public void RemovePortfolio(PortfolioVM portfVm)
        {
            lock (_acctPortfolios)
            {
                int idx = _acctPortfolios.IndexOf(portfVm);
                if (idx >= 0 && idx < _acctPortfolios.Count)
                {
                    _acctPortfolios.RemoveAt(idx);

                    EventAggregator.GetEvent<PortfolioSelectedEvent>().Publish(null);
                }
            }
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

        public ICommand DisconnectCommand
        {
            get;
            private set;
        }

        public ICommand DetachCommand
        {
            get;
            private set;
        }

        public ICommand StartAllPortfolioCmd
        {
            get;
            private set;
        }

        public ICommand StopAllPortfolioCmd
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

        public void ManualCloseOrder(string symbol, trade.TradeDirectionType direction,
            DateTime openDate, int quantity, Action<bool, string> closeDoneHandler)
        {
            Func<string, trade.TradeDirectionType, DateTime, int, OperationResult> closeFunc =
                new Func<string, trade.TradeDirectionType, DateTime, int, OperationResult>
                    (_client.ManualCloseOrder);
            closeFunc.BeginInvoke(symbol, direction, openDate, quantity, 
                delegate(IAsyncResult ar)
                {
                    try
                    {
                        var or = closeFunc.EndInvoke(ar);
                        if (closeDoneHandler != null)
                            closeDoneHandler(or.Success, or.ErrorMessage);
                    }
                    catch (System.Exception ex)
                    {
                        LogManager.Logger.ErrorFormat("Manual close order failed due to {0}", ex.Message);
                        if (closeDoneHandler != null)
                            closeDoneHandler(false, "手动平仓时发生未知错误");
                    }
                }, null);
        }

        public entity.SymbolInfo QuerySymbolInfo(string symbol)
        {
            if (_client.IsConnected)
                return _client.QuerySymbolInfo(symbol);
            else
                return null;
        }

        private void OnAddPortfolio(AccountVM acct)
        {
            PortfolioVM portf = new PortfolioVM(this);
            portf.Id = NextPortfId();
            
            EditPortfolioDlg dlg = new EditPortfolioDlg();
            dlg.Owner = System.Windows.Application.Current.MainWindow;
            dlg.Portfolio = portf;
            bool? res = dlg.ShowDialog();
            if (res ?? false)
            {
                portf.SetHedgeFlag(HedgeFlag);
                PTEntity.PortfolioItem portfolioItem = dlg.Portfolio.GetEntity();
                AddPorfolio(portf);
                if(_client.IsConnected)
                    _client.AddPortfolio(portfolioItem);

                PublishChanged();
            }
        }

        public void StartAccountPortfolios(bool running)
        {
            if (IsConnected)
            {
                foreach (var portf in _acctPortfolios)
                {
                    portf.StartStrategy(running);
                }
            }
        }

        private void OnStartAllPortfolio(AccountVM acct)
        {
            StartAccountPortfolios(true);
        }

        private void OnStopAllPortfolio(AccountVM acct)
        {
            StartAccountPortfolios(false);
        }

        private string NextPortfId()
        {
            if (_acctPortfolios.Count > 0)
                return (int.Parse(_acctPortfolios.Last().Id) + 1).ToString();
            else
                return "1";
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
                        RemovePortfolio(portfVm);
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
            List<PTEntity.PortfolioItem> portfItems = new List<PTEntity.PortfolioItem>();
            foreach (var portf in _acctPortfolios)
            {
                PTEntity.PortfolioItem portfolioItem = portf.GetEntity();
                portfItems.Add(portfolioItem);
            }
            _client.AddPortfCollection(portfItems.ToArray());
        }

        public bool IsConnected
        {
            get { return _status == "已连接"; }
        }

        #region IsBusy
        private bool _isBusy = false;

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


        #region IsExpanded
        private bool _isExpanded;

        public bool IsExpanded
        {
            get { return _isExpanded; }
            set
            {
                if (_isExpanded != value)
                {
                    _isExpanded = value;
                    RaisePropertyChanged("IsExpanded");
                }
            }
        }
        #endregion

        public void Close()
        {
            TradeStaionCutDown();
            _client.Disconnect();
            _host.Exit();
        }

        private int _connectTimes;

        void ChangeStatus(string statusTxt, bool begin)
        {
            Status = statusTxt;
            IsBusy = begin ? true : false;
        }

        private void OnConnectHost(AccountVM acct)
        {
            ChangeStatus("连接中...", true);

            UIContext = SynchronizationContext.Current;

            TradeStationConnector connector = new TradeStationConnector(_client, _clientHandler, UIContext,
                () => new ServerLoginParam
                    {
                        TradeAddress = AddressRepo.EffectiveTrading.Address,
                        QuoteAddress = AddressRepo.EffectiveMarket.Address,
                        BrokerId = acct.BrokerId,
                        InvestorId = acct.InvestorId,
                        UserId = acct.UserId,
                        Password = acct.Password
                    }
                );

            //string localHostIP = NativeHost.GetLocalIP();
            //string host = ConfigurationHelper.GetAppSettingValue("tradeHostIP", localHostIP);
            //HostPort = ConfigurationHelper.GetAppSettingValue("tradeHostPort", 16181);
            
            string effectiveTradeStation = AddressRepo.EffectiveTradeStation.Address;
            string[] addr_port = effectiveTradeStation.Split(':');
            string host = addr_port[0];
            HostPort = int.Parse(addr_port[1]);

            _client.SetPseudo(acct.Id);
            EventLogger.Write(string.Format("正在为{0}建立交易终端...", acct.InvestorId));
            connector.Login(host, HostPort, 
                (succ, error) => 
                {
                    if (succ)
                    {
                        ChangeStatus("已连接", false);
                        EventLogger.Write("连接交易终端成功");
                        SyncToHost();
                    }
                    else
                    {
                        ChangeStatus("未连接", false);
                        EventLogger.Write("连接交易终端失败 : {0}", error);
                    }
                }, 
                status => EventLogger.Write(status));
        }

        private void OnDisconnectHost(AccountVM acct)
        {
            EventLogger.Write(string.Format("正在将{0}关闭交易终端连接...", acct.InvestorId));
            Close();
            EventLogger.Write(string.Format("{0}已断开", acct.InvestorId));
            Status = "未连接";
        }

        private void OnDetachHost(AccountVM acct)
        {
            EventLogger.Write(string.Format("正在将{0}从交易终端断开连接...", acct.InvestorId));
            //_client.Disconnect();
            EventLogger.Write(string.Format("{0}已断开", acct.InvestorId));
            Status = "未连接";
        }

        public void TradeStaionCutDown()
        {
            try
            {
                _client.QuoteLogout();
                _client.TradeLogout();
            }
            catch (System.Exception ex)
            {
                LogManager.Logger.WarnFormat("Cutting down Trade station encountered error - {0}", ex.Message);            	
            }
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

            XAttribute attrUserId = xmlElement.Attribute("userId");
            if (attrUserId != null)
                acct.UserId = attrUserId.Value;

            XAttribute attrPwd = xmlElement.Attribute("password");
            if (attrPwd != null)
                acct.Password = attrPwd.Value;

            XAttribute attrMaxSubmit = xmlElement.Attribute("maxSubmit");
            if (attrMaxSubmit != null)
                acct.MaxSubmit = int.Parse(attrMaxSubmit.Value);

            XAttribute attrMaxCancel = xmlElement.Attribute("maxCancel");
            if (attrMaxCancel != null)
                acct.MaxCancel = int.Parse(attrMaxCancel.Value);

            XAttribute attrHedgeFlag = xmlElement.Attribute("hedgeFlag");
            if (attrHedgeFlag != null)
                acct.HedgeFlag = (PTEntity.HedgeFlagType)Enum.Parse(typeof(PTEntity.HedgeFlagType), attrHedgeFlag.Value);

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
            elem.Add(new XAttribute("userId", _userId));
            elem.Add(new XAttribute("password", _password));
            elem.Add(new XAttribute("maxSubmit", _maxSubmit));
            elem.Add(new XAttribute("maxCancel", _maxCancel));
            elem.Add(new XAttribute("hedgeFlag", _hedgeFlag));

            XElement elemPortfs = new XElement("portfolios");
            lock (_acctPortfolios)
            {
                foreach (var portf in Portfolios)
                {
                    elemPortfs.Add(portf.Persist());
                }
            }
            elem.Add(elemPortfs);

            return elem;
        }

        public event Action<trade.PositionDetailInfo> OnPositionDetailReturn;

        #region Client event handlers

        void _client_OnTradeUpdated(PTEntity.TradeUpdate obj)
        {
            string info = string.Format("trade: {0}\t{1}\t{2}", obj.InstrumentID, obj.Price, obj.TradeTime);
            EventAggregator.GetEvent<TradeUpdatedEvent>().Publish(obj);
        }

        void _client_OnMultiLegOrderUpdated(PTEntity.MultiLegOrder obj)
        {
            string info = string.Format("mlOrder: {0}\t{1}\t{2}", obj.OrderId, obj.PortfolioId, obj.Quantity);
            EventAggregator.GetEvent<MultiLegOrderUpdatedEvent>().Publish(
                new MultiLegOrderUpdateArgs{ AccountId = Id, MultiLegOrder = obj });
        }


        void _client_OnLegOrderUpdated(string portfId, string mlOrderId, string legOrdRef, PTEntity.Order legOrder)
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


        void _clientHandler_OnPortfolioUpdated(PTEntity.PortfolioUpdateItem obj)
        {
            string info = string.Format("Portfolio Updated: {0}\t{1}", obj.Id, obj.Kind);
            Debug.WriteLine(info);
            var portf = Get(obj.Id);
            if (portf != null)
                DispatcherHelper.Current.BeginInvoke(new Action(() => portf.Update(obj)));
        }

        void _client_OnQuoteReceived(entity.Quote obj)
        {
            string info = (string.Format("{0}\t{1}\t{2}", obj.symbol, obj.last, obj.update_time));
        }

        void _client_OnPositionDetialReturn(trade.PositionDetailInfo obj)
        {
            if (OnPositionDetailReturn != null)
            {
                DispatcherHelper.Current.BeginInvoke(
                    new Action(() => OnPositionDetailReturn(obj)));
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

        public bool VerifyStatus()
        {
            if (IsConnected)
                return true;
            else
            {
                MessageBox.Show( 
                    "请先连接帐户!", "帐户错误",
                    MessageBoxButton.OK, MessageBoxImage.Error);
                return false;
            }
        }
    }
}
