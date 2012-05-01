using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.Prism.ViewModel;
using System.ComponentModel.Composition;
using HiFreTradeUI.Win32Invoke;

namespace HiFreTradeUI.ViewModels
{
    [Export]
    public class TradeVM : NotificationObject
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

        private bool ConnectGateway()
        {
            //return Gateway.ConnectTradeAgent("2030", "00092", "888888");
            return true;
        }
    }
}
