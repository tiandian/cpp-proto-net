using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.Prism.ViewModel;
using PortfolioTrading.Utils;

namespace PortfolioTrading.Modules.Portfolio
{
    public class OrderVM : NotificationObject
    {
        public string OrderUid { get; private set; }

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

        #region Direction
        private string _direction;

        public string Direction
        {
            get { return _direction; }
            set
            {
                if (_direction != value)
                {
                    _direction = value;
                    RaisePropertyChanged("Direction");
                }
            }
        }
        #endregion
        
        #region OCFlag
        private string _ocFlag;

        public string OCFlag
        {
            get { return _ocFlag; }
            set
            {
                if (_ocFlag != value)
                {
                    _ocFlag = value;
                    RaisePropertyChanged("OCFlag");
                }
            }
        }
        #endregion

        #region StatusMsg
        private string _statusMsg;

        public string StatusMsg
        {
            get { return _statusMsg; }
            set
            {
                if (_statusMsg != value)
                {
                    _statusMsg = value;
                    RaisePropertyChanged("StatusMsg");
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

        #region Volume
        private int _volume;

        public int Volume
        {
            get { return _volume; }
            set
            {
                if (_volume != value)
                {
                    _volume = value;
                    RaisePropertyChanged("Volume");
                }
            }
        }
        #endregion

        #region VolTraded
        private int _volTraded;

        public int VolTraded
        {
            get { return _volTraded; }
            set
            {
                if (_volTraded != value)
                {
                    _volTraded = value;
                    RaisePropertyChanged("VolTraded");
                }
            }
        }
        #endregion

        #region OrderPriceType
        private string _orderPriceType;

        public string OrderPriceType
        {
            get { return _orderPriceType; }
            set
            {
                if (_orderPriceType != value)
                {
                    _orderPriceType = value;
                    RaisePropertyChanged("OrderPriceType");
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

        #region IsFinished
        private bool _isFinished;

        public bool IsFinished
        {
            get { return _isFinished; }
            set
            {
                if (_isFinished != value)
                {
                    _isFinished = value;
                    RaisePropertyChanged("IsFinished");
                }
            }
        }
        #endregion


        public void From(trade.Order order, bool fromParent = false)
        {
            OrderUid = GetOrderUid(order);

            Symbol = order.InstrumentID;
            Direction = GetDirection(order.Direction);
            OCFlag = GetOCFlag(order.CombOffsetFlag);
            StatusMsg = GetStatus(order.OrderSubmitStatus, order.OrderStatus);
            IsFinished = CheckForFinish(order.OrderSubmitStatus, order.OrderStatus);
            if(!string.IsNullOrEmpty(order.InsertTime))
                InsertTime = DateTime.Parse(order.InsertTime);
            Volume = order.VolumeTotalOriginal;
            VolTraded = order.VolumeTraded;
            OrderPriceType = GetPriceType(order.OrderPriceType);
            LimitPrice = order.LimitPrice;

            if (!fromParent)
            {
                if (order.OrderSubmitStatus > trade.OrderSubmitStatusType.ACCEPTED)
                {
                    EventLogger.Write("{0} {1} 被拒绝 -({2})", Direction, Symbol, order.StatusMsg);
                }
            }
        }

        public static string GetOCFlag(string offsetFlag)
        {
            char of = offsetFlag[0];
            int i_of = (int)of;
            switch (i_of)
            {
                case 48:
                    return "开仓";
                case 49:
                    return "平仓";
                case 50:
                    return "强平";
                case 51:
                    return "平今";
                case 52:
                    return "平昨";
                case 53:
                    return "强减";
                case 54:
                    return "本地强平";
                default:
                    return "未知";
            }
        }

        private static bool CheckForFinish(trade.OrderSubmitStatusType submitStatus,
                                       trade.OrderStatusType statusType)
        {
            switch (submitStatus)
            {
                case trade.OrderSubmitStatusType.INSERT_SUBMITTED:
                case trade.OrderSubmitStatusType.ACCEPTED:
                    {
                        switch (statusType)
                        {
                            case trade.OrderStatusType.ALL_TRADED:
                            case trade.OrderStatusType.ORDER_CANCELED:
                                return true;
                        }
                    }
                    break;
                default:
                    return false;
            }
            return false;
        }

        public static string GetStatus(trade.OrderSubmitStatusType submitStatus,
                                       trade.OrderStatusType statusType)
        {
            string status = "未知";

            switch (submitStatus)
            {
                case trade.OrderSubmitStatusType.NOT_SUBMITTED:
                    status = "未提交";
                    break;
                case trade.OrderSubmitStatusType.INSERT_SUBMITTED:
                case trade.OrderSubmitStatusType.ACCEPTED:
                    {
                        if (submitStatus == trade.OrderSubmitStatusType.INSERT_SUBMITTED)
                            status = "委托已提交";
                        else if (submitStatus == trade.OrderSubmitStatusType.ACCEPTED)
                            status = "委托被接受";

                        switch (statusType)
                        {
                            case trade.OrderStatusType.ALL_TRADED:
                                status = "全部成交";
                                break;
                            case trade.OrderStatusType.PART_TRADED_QUEUEING:
                            case trade.OrderStatusType.PART_TRADED_NOT_QUEUEING:
                                status = "部分成交";
                                break;
                            case trade.OrderStatusType.NO_TRADE_QUEUEING:
                            case trade.OrderStatusType.NO_TRADE_NOT_QUEUEING:
                                status = "未成交";
                                break;
                            case trade.OrderStatusType.ORDER_CANCELED:
                                status = "委托已取消";
                                break;
                            case trade.OrderStatusType.ORDER_NOT_TOUCHED:
                                status = "委托未触发";
                                break;
                            case trade.OrderStatusType.ORDER_TOUCHED:
                                status = "委托已触发";
                                break;
                            case trade.OrderStatusType.STATUS_UNKNOWN:
                            default:
                                break;
                        }
                    }
                    break;
                case trade.OrderSubmitStatusType.CANCEL_SUBMITTED:
                    status = "取消已提交";
                    break;
                case trade.OrderSubmitStatusType.MODIFY_SUBMITTED:
                    status = "修改已提交";
                    break;
                case trade.OrderSubmitStatusType.INSERT_REJECTED:
                    status = "委托被拒绝";
                    break;
                case trade.OrderSubmitStatusType.CANCEL_REJECTED:
                    status = "取消被拒绝";
                    break;
                case trade.OrderSubmitStatusType.MODIFY_REJECTED:
                    status = "修改被拒绝";
                    break;
                default:
                    break;
            }

            return status;
        }

        public static string GetDirection(trade.TradeDirectionType direction)
        {
            switch (direction)
            {
                case trade.TradeDirectionType.BUY:
                    return "买入";
                case trade.TradeDirectionType.SELL:
                    return "卖出";
                default:
                    return "未知";
            }    
        }

        public static string GetPriceType(trade.OrderPriceTypeType priceType)
        {
            switch (priceType)
            {
                case trade.OrderPriceTypeType.ANY_PRICE:
                    return "市价";
                case trade.OrderPriceTypeType.LIMIT_PRICE:
                    return "限价";
                case trade.OrderPriceTypeType.BEST_PRICE:
                    return "最好";
                case trade.OrderPriceTypeType.LAST_PRICE:
                    return "最新";
                case trade.OrderPriceTypeType.LAST_PRICE_PLUS_ONE_TICKS:
                    return "最新+1";
                case trade.OrderPriceTypeType.LAST_PRICE_PLUS_TWO_TICKS:
                    return "最新+2";
                case trade.OrderPriceTypeType.LAST_PRICE_PLUS_THREE_TICKS:
                    return "最新+3";
                case trade.OrderPriceTypeType.ASK_PRICE1:
                    return "卖一";
                case trade.OrderPriceTypeType.ASK_PRICE1_PLUS_ONE_TICKS:
                    return "卖一+1";
                case trade.OrderPriceTypeType.ASK_PRICE1_PLUS_TWO_TICKS:
                    return "卖一+2";
                case trade.OrderPriceTypeType.ASK_PRICE1_PLUS_THREE_TICKS:
                    return "卖一+3";
                case trade.OrderPriceTypeType.BID_PRICE1:
                    return "买一";
                case trade.OrderPriceTypeType.BID_PRICE1_PLUS_ONE_TICKS:
                    return "买一+1";
                case trade.OrderPriceTypeType.BID_PRICE1_PLUS_TWO_TICKS:
                    return "买一+2";
                case trade.OrderPriceTypeType.BID_PRICE1_PLUS_THREE_TICKS:
                    return "买一+3";
                default:
                    return "未知";
            }
        }

        public static string GetOrderUid(trade.Order order)
        {
            string directionIndicator = order.Direction == trade.TradeDirectionType.BUY ?
                "B" : "S";
            return string.Format("{0}-{1}", directionIndicator, order.InstrumentID);
        }
    }
}
