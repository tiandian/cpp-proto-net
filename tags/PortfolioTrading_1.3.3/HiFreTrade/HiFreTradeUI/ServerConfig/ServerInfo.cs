using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.Prism.ViewModel;

namespace HiFreTradeUI.ServerConfig
{
    public enum ServerStatus { Unknown, Testing, Available, Failed }

    public class ServerInfo : NotificationObject
    {
        public string Name { get; set; }
        public string Address { get; set; }
        public bool IsReal { get; set; }

        #region Status
        private ServerStatus _status = ServerStatus.Unknown;

        public ServerStatus Status
        {
            get { return _status; }
            set
            {
                if (_status != value)
                {
                    _status = value;
                    RaisePropertyChanged("Status");
                }
            }
        }
        #endregion

    }
}
