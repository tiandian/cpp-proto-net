using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.Prism.ViewModel;
using System.ComponentModel.Composition;
using Win32 = HiFreTradeUI.Win32Invoke;

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

        private static Win32.Gateway.OperationRecordUpdateDelegate recordsUpdateFunc;
        private static Win32.Gateway.TimeNSalesUpdateDelegate tnsDataUpdateFunc;

        private bool ConnectGateway()
        {
            recordsUpdateFunc = new Win32.Gateway.OperationRecordUpdateDelegate(OnOperationRecordsUpdate);
            tnsDataUpdateFunc = new Win32.Gateway.TimeNSalesUpdateDelegate(OnTimeNSalesUpdate);
            return Win32.Gateway.ConnectTradeAgent("0240", "0240050002", "888888", recordsUpdateFunc, tnsDataUpdateFunc);
        }

        public void OnOperationRecordsUpdate(Win32.OperationRecord record)
        {

        }

        public void OnTimeNSalesUpdate(Win32.TimeNSalesData tnsData)
        {

        }

        #region RectPeriod
        private int _rectPeriod = 180;

        public int RectPeriod
        {
            get { return _rectPeriod; }
            set
            {
                if (_rectPeriod != value)
                {
                    _rectPeriod = value;
                    RaisePropertyChanged("RectPeriod");
                }
            }
        }
        #endregion

        #region RectRange
        private int _rectRange = 5;

        public int RectRange
        {
            get { return _rectRange; }
            set
            {
                if (_rectRange != value)
                {
                    _rectRange = value;
                    RaisePropertyChanged("RectRange");
                }
            }
        }
        #endregion

        #region UpBreak
        private bool _upBreak = true;

        public bool UpBreak
        {
            get { return _upBreak; }
            set
            {
                if (_upBreak != value)
                {
                    _upBreak = value;
                    RaisePropertyChanged("UpBreak");
                }
            }
        }
        #endregion

        #region DownBreak
        private bool _downBreak = true;

        public bool DownBreak
        {
            get { return _downBreak; }
            set
            {
                if (_downBreak != value)
                {
                    _downBreak = value;
                    RaisePropertyChanged("DownBreak");
                }
            }
        }
        #endregion

        #region BreakoutCriterion
        private int _breakCriterion = 3;

        public int BreakoutCriterion
        {
            get { return _breakCriterion; }
            set
            {
                if (_breakCriterion != value)
                {
                    _breakCriterion = value;
                    RaisePropertyChanged("BreakoutCriterion");
                }
            }
        }
        #endregion

        #region BreakoutTimespan
        private int _breakoutTimespan = 3;

        public int BreakoutTimespan
        {
            get { return _breakoutTimespan; }
            set
            {
                if (_breakoutTimespan != value)
                {
                    _breakoutTimespan = value;
                    RaisePropertyChanged("BreakoutTimespan");
                }
            }
        }
        #endregion

        #region GainLimit
        private int _gainLimit = 8;

        public int GainLimit
        {
            get { return _gainLimit; }
            set
            {
                if (_gainLimit != value)
                {
                    _gainLimit = value;
                    RaisePropertyChanged("GainLimit");
                }
            }
        }
        #endregion

        #region LossLimit
        private int _lossLimit = 3;

        public int LossLimit
        {
            get { return _lossLimit; }
            set
            {
                if (_lossLimit != value)
                {
                    _lossLimit = value;
                    RaisePropertyChanged("LossLimit");
                }
            }
        }
        #endregion

    }
}
