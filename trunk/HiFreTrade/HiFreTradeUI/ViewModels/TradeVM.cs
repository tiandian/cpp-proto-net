using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.Prism.ViewModel;
using System.ComponentModel.Composition;
using Win32 = HiFreTradeUI.Win32Invoke;
using Microsoft.Practices.ServiceLocation;
using Microsoft.Practices.Prism.Events;
using HiFreTradeUI.Events;

namespace HiFreTradeUI.ViewModels
{
    [Export]
    public class TradeVM : NotificationObject
    {
        private IEventAggregator EventAggregator { get; set; }
        public TradeVM()
        {
            EventAggregator = ServiceLocator.Current.GetInstance<IEventAggregator>();
        }

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

        public void Disconnect()
        {
            Win32.Gateway.DisconnectTradeAgent();
        }

        private static Win32.Gateway.OperationRecordUpdateDelegate recordsUpdateFunc;
        private static Win32.Gateway.TimeNSalesUpdateDelegate tnsDataUpdateFunc;

        private bool ConnectGateway()
        {
            recordsUpdateFunc = new Win32.Gateway.OperationRecordUpdateDelegate(OnOperationRecordsUpdate);
            tnsDataUpdateFunc = new Win32.Gateway.TimeNSalesUpdateDelegate(OnTimeNSalesUpdate);
            return Win32.Gateway.ConnectTradeAgent("0240", "0240050003", "888888", recordsUpdateFunc, tnsDataUpdateFunc);
        }

        private static string GetDirection(int iDirection)
        {
            /* iDirection
            #define SHORT_BREAKOUT 1
            #define LONG_BREAKOUT 2 */
            switch(iDirection)
            {
                case 0:
                    return "未突破";
                case 1:
                    return "空头突破";
                case 2:
                    return "多头突破";
                default:
                    throw new ArgumentException(string.Format("Invalid given direction({0})", iDirection));
            }
        }

        private static string GetEntryReason(int iEntryReason)
        {
            /*
            // iEntryReason
            #define CONDITION_TRIGGER 1
            #define MANUAL_OPEN 2
            */
            switch(iEntryReason)
            {
                case 0:
                    return "未开仓";
                case 1:
                    return "条件触发";
                case 2:
                    return "手动开仓";
                default:
                    throw new ArgumentException(string.Format("Invalid entry reason({0})", iEntryReason));
            }
        }

        private static string GetExitReason(int iExitReason)
        {
            /*
            // iExitReason
            #define STOP_GAIN 1
            #define STOP_LOSS 2
            #define MANUAL_CLOSE 3
            */
            switch(iExitReason)
            {
                case 0:
                    return "未平仓";
                case 1:
                    return "止盈";
                case 2:
                    return "止损";
                case 3:
                    return "手动平仓";
                default:
                    throw new ArgumentException(string.Format("Invalid exit reason({0})", iExitReason));
            }
        }

        private static string GetEntryExitType(int iType)
        {
            /*
            // iEntryType and iExitType
            #define SHORT_OPEN 1
            #define LONG_OPEN 2
            #define SHORT_CLOSE 3
            #define LONG_CLOSE 4
             */
            switch(iType)
            {
                case 0:
                    return "未知";
                case 1:
                    return "空开";
                case 2:
                    return "多开";
                case 3:
                    return "空平";
                case 4:
                    return "多平";
                default:
                    throw new ArgumentException(string.Format("Invalid entry or exit type ({0})", iType));
            }
        }

        private static string GetEntryExitStatus(int iStatus)
        {
            /*
            // iEntryStatus and iExitStatus
            #define UNOPEN 1
            #define ORDER_SUBMIT 2
            #define PENDING 3
            #define PARTIALLY_FILLED 4
            #define FULL_FILLED 5
            #define CANCELED 6
            #define REJECTED 7
            #define CLOSED UNOPEN
             */
            switch(iStatus)
            {
                case 0:
                    return "未知";
                case 1:
                    return "未开仓";
                case 2:
                    return "已提交";
                case 3:
                    return "等待成交";
                case 4:
                    return "部分成交";
                case 5:
                    return "全部成交";
                case 6:
                    return "已撤单";
                case 7:
                    return "被拒绝";
                default:
                    throw new ArgumentException(string.Format("Invalid entry or exit status ({0})", iStatus));
            }
        }

        public void OnOperationRecordsUpdate(Win32.OperationRecord win32Record)
        {
            OperationRecord record = new OperationRecord();

            record.RecordID = win32Record.iRecordId;
            record.Symbol = win32Record.caSymbol;
            DateTime time;
            bool succ = DateTime.TryParse(win32Record.caRectPeriodBegin, out time);
            if(succ) record.RectPeriodBegin = time;
            succ = DateTime.TryParse(win32Record.caRectPeriodEnd, out time);
            if(succ) record.RectPeriodEnd = time;

            record.UpperBoundary = win32Record.dUpperBoundary;
            record.LowerBoundary = win32Record.dLowerBoundary;
            record.Range = win32Record.dRange;
            record.Direction = GetDirection(win32Record.iDirection);

            record.EntryPoint = win32Record.dEntryPoint;
            succ = DateTime.TryParse(win32Record.caEntryTime, out time);
            if(succ) record.EntryTime = time;
            record.EntryReason = GetEntryReason(win32Record.iEntryReason);
            record.EntryType = GetEntryExitType(win32Record.iEntryType);
            record.EntryQuantity = win32Record.iEntryQuantity;
            record.EntryStatus = GetEntryExitStatus(win32Record.iEntryStatus);

            record.ExitPoint = win32Record.dExitPoint;
            succ = DateTime.TryParse(win32Record.caExitTime, out time);
            if(succ) record.ExitTime = time;
            record.ExitReason = GetExitReason(win32Record.iExitReason);
            record.ExitType = GetEntryExitType(win32Record.iExitType);
            record.ExitQuantity = win32Record.iExitQuantity;
            record.ExitStatus = GetEntryExitStatus(win32Record.iExitStatus);

            record.ProfitLoss = win32Record.ProfitLoss;

            EventAggregator.GetEvent<OperRecordUpdateEvent>().Publish(record);
        }

        public void OnTimeNSalesUpdate(Win32.TimeNSalesData tnsPacket)
        {
            TimeNSalesData tnsData = new TimeNSalesData();
            tnsData.Symbol = tnsPacket.caSymbol;
            DateTime time;
            bool succ = DateTime.TryParse(tnsPacket.caTimeStamp, out time);
            tnsData.TimeStamp = time;
            tnsData.Price = tnsPacket.dPrice;
            tnsData.Quantity = tnsPacket.iQuantity;
            tnsData.Direction = GetEntryExitType(tnsPacket.iDirection);

            EventAggregator.GetEvent<TimeNSalesUpdateEvent>().Publish(tnsData);
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
