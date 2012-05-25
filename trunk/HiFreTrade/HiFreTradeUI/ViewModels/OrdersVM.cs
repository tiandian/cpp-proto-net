using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel.Composition;
using Microsoft.Practices.Prism.ViewModel;
using System.Collections.ObjectModel;

namespace HiFreTradeUI.ViewModels
{
    [Export]
    public class OrdersVM
    {
        private ObservableCollection<OrderItem> _orders = new ObservableCollection<OrderItem>();

        public IEnumerable<OrderItem> OrderItems
        {
            get { return _orders; }
        }

        public void Clear()
        {
            _orders.Clear();
        }

        public void AddOrder(Win32Invoke.OrderInfo orderInfo)
        {
            OrderItem order = new OrderItem();

            order.Symbol = orderInfo.caSymbol;
            order.OrderRef = orderInfo.caOrderRef;
            order.InvestorID = orderInfo.caInvestorID;
            order.UserID = orderInfo.caUserID;
            order.HedgeFlag = GetHedgeFlag(orderInfo.cHedgeFlag);
            order.EntryExitType = TradeVM.GetEntryExitType(orderInfo.iEntryExitType);
            order.PriceType = GetPriceType(orderInfo.cPriceType);
            order.LimitPrice = orderInfo.dLimitPrice;
            order.VolumeTotalOriginal = orderInfo.iVolumeTotalOriginal;
            order.ExchangeID = orderInfo.caExchangeID;
            order.OrderStatus = TradeVM.GetEntryExitStatus(orderInfo.iOrderStatus);
            order.OrderSysID = orderInfo.caOrderSysID;
            order.VolumeTraded = orderInfo.iVolumeTraded;
            order.VolumeRemain = orderInfo.iVolumeRemain;
            order.InsertDate = PositionDetailVM.GetDate(orderInfo.caInsertDate);
            DateTime time;
            bool succ = DateTime.TryParse(orderInfo.caInsertTime, out time);
            if (succ) order.InsertTime = time;
            succ = DateTime.TryParse(orderInfo.caUpdateTime, out time);
            if (succ) order.UpdateTime = time;
            succ = DateTime.TryParse(orderInfo.caCancelTime, out time);
            if (succ) order.CancelTime = time;
            order.FrontID = orderInfo.iFrontID;
            order.SessionID = orderInfo.iSessionID;

            _orders.Add(order);
        }

        private static string GetHedgeFlag(char hedgeFlag)
        {
            string hedgeText = string.Empty;

            switch (hedgeFlag)
            {
                case '1':
                    hedgeText = "套保";
                    break;
                case '2':
                    hedgeText = "套利";
                    break;
                case '3':
                    hedgeText = "投机";
                    break;
            }

            return hedgeText;
        }

        private static string GetPriceType(char priceType)
        {
            string priceTypeText = string.Empty;

            switch(priceType)
            {
                case '1':
                    priceTypeText = "任意价";
                    break;
                case '2':
                    priceTypeText = "限价";
                    break;
                case '3':
                    priceTypeText = "最优价";
                    break;
                case '4':
                    priceTypeText = "最新价";
                    break;
                default:
                    priceTypeText = "其它";
                    break;
            }

            return priceTypeText;
        }
    }

    public class OrderItem : NotificationObject
    {
        #region Symbol
        private string _symbol;

        public string Symbol
        {
            get { return _symbol; }
            set
            {
                if (_symbol != value)
                {
                    _symbol = value;
                    RaisePropertyChanged("Symbol");
                }
            }
        }
        #endregion

        #region OrderRef
        private string _orderRef;

        public string OrderRef
        {
            get { return _orderRef; }
            set
            {
                if (_orderRef != value)
                {
                    _orderRef = value;
                    RaisePropertyChanged("OrderRef");
                }
            }
        }
        #endregion

        #region InvestorID
        private string _investorID;

        public string InvestorID
        {
            get { return _investorID; }
            set
            {
                if (_investorID != value)
                {
                    _investorID = value;
                    RaisePropertyChanged("InvestorID");
                }
            }
        }
        #endregion

        #region UserID
        private string _userId;

        public string UserID
        {
            get { return _userId; }
            set
            {
                if (_userId != value)
                {
                    _userId = value;
                    RaisePropertyChanged("UserID");
                }
            }
        }
        #endregion

        #region HedgeFlag
        private string _hedgeFlag;

        public string HedgeFlag
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

        #region EntryExitType
        private string _entryExit;

        public string EntryExitType
        {
            get { return _entryExit; }
            set
            {
                if (_entryExit != value)
                {
                    _entryExit = value;
                    RaisePropertyChanged("EntryExitType");
                }
            }
        }
        #endregion

        #region PriceType
        private string _priceType;

        public string PriceType
        {
            get { return _priceType; }
            set
            {
                if (_priceType != value)
                {
                    _priceType = value;
                    RaisePropertyChanged("PriceType");
                }
            }
        }
        #endregion

        #region LimitPrice
        private double _limitPrice;

        public double LimitPrice
        {
            get { return _limitPrice; }
            set
            {
                if (_limitPrice != value)
                {
                    _limitPrice = value;
                    RaisePropertyChanged("LimitPrice");
                }
            }
        }
        #endregion

        #region VolumeTotalOriginal
        private int _volume;

        public int VolumeTotalOriginal
        {
            get { return _volume; }
            set
            {
                if (_volume != value)
                {
                    _volume = value;
                    RaisePropertyChanged("VolumeTotalOriginal");
                }
            }
        }
        #endregion

        #region ExchangeID
        private string _exchangeID;

        public string ExchangeID
        {
            get { return _exchangeID; }
            set
            {
                if (_exchangeID != value)
                {
                    _exchangeID = value;
                    RaisePropertyChanged("ExchangeID");
                }
            }
        }
        #endregion

        #region OrderStatus
        private string _orderStatus;

        public string OrderStatus
        {
            get { return _orderStatus; }
            set
            {
                if (_orderStatus != value)
                {
                    _orderStatus = value;
                    RaisePropertyChanged("OrderStatus");
                }
            }
        }
        #endregion

        #region OrderSysID
        private string _orderSysId;

        public string OrderSysID
        {
            get { return _orderSysId; }
            set
            {
                if (_orderSysId != value)
                {
                    _orderSysId = value;
                    RaisePropertyChanged("OrderSysID");
                }
            }
        }
        #endregion

        #region VolumeTraded
        private int _volTraded;

        public int VolumeTraded
        {
            get { return _volTraded; }
            set
            {
                if (_volTraded != value)
                {
                    _volTraded = value;
                    RaisePropertyChanged("VolumeTraded");
                }
            }
        }
        #endregion

        #region VolumeRemain
        private int _volRemain;

        public int VolumeRemain
        {
            get { return _volRemain; }
            set
            {
                if (_volRemain != value)
                {
                    _volRemain = value;
                    RaisePropertyChanged("VolumeRemain");
                }
            }
        }
        #endregion

        #region InsertDate
        private DateTime _insertDate;

        public DateTime InsertDate
        {
            get { return _insertDate; }
            set
            {
                if (_insertDate != value)
                {
                    _insertDate = value;
                    RaisePropertyChanged("InsertDate");
                }
            }
        }
        #endregion

        #region InsertTime
        private DateTime _insertTime;

        public DateTime InsertTime
        {
            get { return _insertTime; }
            set
            {
                if (_insertTime != value)
                {
                    _insertTime = value;
                    RaisePropertyChanged("InsertTime");
                }
            }
        }
        #endregion

        #region UpdateTime
        private DateTime _updateTime;

        public DateTime UpdateTime
        {
            get { return _updateTime; }
            set
            {
                if (_updateTime != value)
                {
                    _updateTime = value;
                    RaisePropertyChanged("UpdateTime");
                }
            }
        }
        #endregion

        #region CancelTime
        private DateTime _cancelTime;

        public DateTime CancelTime
        {
            get { return _cancelTime; }
            set
            {
                if (_cancelTime != value)
                {
                    _cancelTime = value;
                    RaisePropertyChanged("CancelTime");
                }
            }
        }
        #endregion

        #region FrontID
        private int _frontId;

        public int FrontID
        {
            get { return _frontId; }
            set
            {
                if (_frontId != value)
                {
                    _frontId = value;
                    RaisePropertyChanged("FrontID");
                }
            }
        }
        #endregion

        #region SessionID
        private int _sessionId;

        public int SessionID
        {
            get { return _sessionId; }
            set
            {
                if (_sessionId != value)
                {
                    _sessionId = value;
                    RaisePropertyChanged("SessionID");
                }
            }
        }
        #endregion
    }
}
