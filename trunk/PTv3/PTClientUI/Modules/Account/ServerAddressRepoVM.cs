using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.Prism.ViewModel;
using System.ComponentModel.Composition;
using System.Xml.Linq;
using System.IO;
using PortfolioTrading.Infrastructure;
using PortfolioTrading.Utils;

namespace PortfolioTrading.Modules.Account
{
    [Export]
    public class ServerAddressRepoVM : NotificationObject
    {
        private List<ServerAddress> _mkt_servers = new List<ServerAddress>();
        private List<ServerAddress> _td_servers = new List<ServerAddress>();
        private List<ServerAddress> _trade_stations = new List<ServerAddress>();

        public IEnumerable<ServerAddress> MarketServers
        {
            get { return _mkt_servers; }
        }

        public IEnumerable<ServerAddress> TradingServers
        {
            get { return _td_servers; }
        }

        public IEnumerable<ServerAddress> TradeStations
        {
            get { return _trade_stations;  }
        }

        #region EffectiveMarket
        private ServerAddress _effectMarket;

        public ServerAddress EffectiveMarket
        {
            get { return _effectMarket; }
            set
            {
                if (_effectMarket != value)
                {
                    _effectMarket = value;
                    RaisePropertyChanged("EffectiveMarket");
                }
            }
        }
        #endregion

        #region EffectiveTrading
        private ServerAddress _effectiveTrading;

        public ServerAddress EffectiveTrading
        {
            get { return _effectiveTrading; }
            set
            {
                if (_effectiveTrading != value)
                {
                    _effectiveTrading = value;
                    RaisePropertyChanged("EffectiveTrading");
                }
            }
        }
        #endregion

        #region EffectiveTradeStation
        private ServerAddress _effectiveTradeStation;

        public ServerAddress EffectiveTradeStation
        {
            get { return _effectiveTradeStation; }
            set
            {
                if (_effectiveTradeStation != value)
                {
                    _effectiveTradeStation = value;
                    RaisePropertyChanged("EffectiveTradeStation");
                }
            }
        }
        #endregion

        private readonly string BrokersXmlPath = "brokers.xml";
        public void LoadServerList()
        {
            _mkt_servers.Add(new ServerAddress { Name = "模拟行情", Address = "tcp://ctpsim-front01.gfqh.cn:43213" });
            _td_servers.Add(new ServerAddress { Name = "模拟交易", Address = "tcp://ctpsim-front01.gfqh.cn:43205" });

            _mkt_servers.Add(new ServerAddress { Name = "飞马仿真", Address = "tcp://117.184.207.110:17101" });
            _td_servers.Add(new ServerAddress { Name = "华泰仿真", Address = "tcp://117.184.207.110:17061" });
            _td_servers.Add(new ServerAddress { Name = "国泰仿真", Address = "tcp://117.184.207.110:17131" });


            if (File.Exists(BrokersXmlPath))
            {
                XElement rootElem = XElement.Load(BrokersXmlPath);
                var serversElemCollection = rootElem.Descendants("Servers");
                var serversElem = serversElemCollection.FirstOrDefault();
                var elemServers = serversElem.Elements("Server");
                foreach (var elemServ in elemServers)
                {
                    string name = elemServ.Element("Name").Value;
                    var tradingItems = GetAddressItems(elemServ.Element("Trading"));
                    FillList(_td_servers, tradingItems, name);
                    var marketItems = GetAddressItems(elemServ.Element("MarketData"));
                    FillList(_mkt_servers, marketItems, name);
                }
            }

            _trade_stations.Add(new ServerAddress { Name = "交易服务42", Address = ConfigurationHelper.GetAppSettingValue("42", "180.166.182.12:62248") });
            _trade_stations.Add(new ServerAddress { Name = "交易服务47", Address = ConfigurationHelper.GetAppSettingValue("47", "180.166.182.12:62249") });
            _trade_stations.Add(new ServerAddress { Name = "交易服务61(万兆)", Address = ConfigurationHelper.GetAppSettingValue("61", "180.166.182.12:62263") });

            string localHostIP = NativeHost.GetLocalIP();
            string localPort = "16181";
            _trade_stations.Add(new ServerAddress { Name = "本地服务", Address = string.Format("{0}:{1}", 
                localHostIP, localPort)});
        }

        public void SelectMarket(string name)
        {
            ServerAddress addr = _mkt_servers.Find(s => s.Name == name);
            EffectiveMarket = addr != null ? addr : _mkt_servers.FirstOrDefault();
        }

        public void SelectTrading(string name)
        {
            ServerAddress addr = _td_servers.Find(s => s.Name == name);
            EffectiveTrading = addr != null ? addr : _td_servers.FirstOrDefault();
        }

        public void SelectTradeStation(string name)
        {
            ServerAddress addr = _trade_stations.Find(s => s.Name == name);
            EffectiveTradeStation = addr != null ? addr : _trade_stations.FirstOrDefault();
        }

        private IEnumerable<XElement> GetAddressItems(XElement elem)
        {
            if (elem != null)
                return elem.Elements("item");
            return null;
        }

        private void FillList(List<ServerAddress> serverList, IEnumerable<XElement> elemItems, string namePrefix)
        {
            if (elemItems != null)
            {
                int idx = 1;
                foreach (var item in elemItems)
                {
                    ServerAddress svrAddr = new ServerAddress();
                    svrAddr.Name = string.Format("{0} {1}", namePrefix, idx++);
                    if (item.Value.StartsWith("udp"))
                        svrAddr.Address = item.Value;
                    else
                        svrAddr.Address = "tcp://" + item.Value;
                    serverList.Add(svrAddr);
                }
            }
        }
    }

    public class ServerAddress : NotificationObject
    {
        #region Name
        private string _name;

        public string Name
        {
            get { return _name; }
            set
            {
                if (_name != value)
                {
                    _name = value;
                    RaisePropertyChanged("Name");
                }
            }
        }
        #endregion

        #region Address
        private string _addr;

        public string Address
        {
            get { return _addr; }
            set
            {
                if (_addr != value)
                {
                    _addr = value;
                    RaisePropertyChanged("Address");
                }
            }
        }
        #endregion

    }
}
