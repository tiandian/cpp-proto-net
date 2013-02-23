using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.Prism.ViewModel;
using System.ComponentModel.Composition;

namespace PortfolioTrading.Modules.Account
{
    [Export]
    public class ServerAddressRepoVM : NotificationObject
    {
        private List<ServerAddress> _mkt_servers = new List<ServerAddress>();
        private List<ServerAddress> _td_servers = new List<ServerAddress>();

        public IEnumerable<ServerAddress> MarketServers
        {
            get { return _mkt_servers; }
        }

        public IEnumerable<ServerAddress> TradingServers
        {
            get { return _td_servers; }
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

        public void LoadServerList()
        {
            _mkt_servers.Add(new ServerAddress { Name = "模拟行情", Address = "tcp://ctpsim-front01.gfqh.cn:43213" });
            _td_servers.Add(new ServerAddress { Name = "模拟交易", Address = "tcp://ctpsim-front01.gfqh.cn:43205" });
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
