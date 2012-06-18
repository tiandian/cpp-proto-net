using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections.ObjectModel;
using Microsoft.Practices.Prism.ViewModel;

namespace HiFreTradeUI.ServerConfig
{
    public class ServerGroup : NotificationObject
    {
        private ObservableCollection<ServerInfo> _servers = new ObservableCollection<ServerInfo>();

        public ObservableCollection<ServerInfo> Servers
        {
            get { return _servers; }
        }

        #region Name
        private string _name;

        public string GroupName
        {
            get { return _name; }
            set
            {
                if (_name != value)
                {
                    _name = value;
                    RaisePropertyChanged("GroupName");
                }
            }
        }
        #endregion
    }
}
