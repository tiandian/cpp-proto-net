﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.Prism.ViewModel;
using System.Collections.ObjectModel;
using PortfolioTrading.Utils;

namespace PortfolioTrading.Modules.Portfolio
{
    public class MultiLegOrderVM : NotificationObject
    {
        private ObservableCollection<OrderVM> _orders = new ObservableCollection<OrderVM>();

        public trade.MultiLegOrder LastOrder { get; private set; }

        public MultiLegOrderVM(string acctId)
        {
            this.AccountId = acctId;
        }

        public string AccountId { get; private set; }

        #region OrderId
        private string _orderId;

        public string OrderId
        {
            get { return _orderId; }
            set
            {
                if (_orderId != value)
                {
                    _orderId = value;
                    RaisePropertyChanged("OrderId");
                }
            }
        }
        #endregion

        #region PortfolioId
        private string _portfolioId;

        public string PortfolioId
        {
            get { return _portfolioId; }
            set
            {
                if (_portfolioId != value)
                {
                    _portfolioId = value;
                    RaisePropertyChanged("PortfolioId");
                }
            }
        }
        #endregion

        #region Quantity
        private int _quantity;

        public int Quantity
        {
            get { return _quantity; }
            set
            {
                if (_quantity != value)
                {
                    _quantity = value;
                    RaisePropertyChanged("Quantity");
                }
            }
        }
        #endregion

        #region Profit
        private double _profit;

        public double Profit
        {
            get { return _profit; }
            set
            {
                if (_profit != value)
                {
                    _profit = value;
                    RaisePropertyChanged("Profit");
                }
            }
        }
        #endregion

        #region IsOpenOrder
        private bool _isOpenOrder;

        public bool IsOpenOrder
        {
            get { return _isOpenOrder; }
            set
            {
                if (_isOpenOrder != value)
                {
                    _isOpenOrder = value;
                    RaisePropertyChanged("IsOpenOrder");
                }
            }
        }
        #endregion

        #region IsAllFinished
        private bool _isAllFinished;

        public bool IsAllFinished
        {
            get { return _isAllFinished; }
            set
            {
                if (_isAllFinished != value)
                {
                    _isAllFinished = value;
                    RaisePropertyChanged("IsAllFinished");
                }
            }
        }
        #endregion

        #region OpenOrderId
        private string _openOrderId;

        public string OpenOrderId
        {
            get { return _openOrderId; }
            set
            {
                if (_openOrderId != value)
                {
                    _openOrderId = value;
                    RaisePropertyChanged("OpenOrderId");
                }
            }
        }
        #endregion

        #region Reason
        private string _reason;

        public string Reason
        {
            get { return _reason; }
            set
            {
                if (_reason != value)
                {
                    _reason = value;
                    RaisePropertyChanged("Reason");
                }
            }
        }
        #endregion


        public IEnumerable<OrderVM> Legs
        {
            get { return _orders; }
        }

        public void From(trade.MultiLegOrder mlOrder)
        {
            LastOrder = mlOrder;

            OrderId = mlOrder.OrderId;
            OpenOrderId = mlOrder.OpenOrderId;
            Reason = GetReasonDisplayText(mlOrder.Reason);
            PortfolioId = mlOrder.PortfolioId;
            Quantity = mlOrder.Quantity;
            IsOpenOrder = mlOrder.OrderId == mlOrder.OpenOrderId;

            bool allFinished = false;
            for (int i = 0; i < mlOrder.Legs.Count; ++i )
            {
                if (_orders.Count == i)
                    _orders.Add(new OrderVM());

                var legOrder = mlOrder.Legs[i];
                _orders[i].From(legOrder, true);
                allFinished = _orders[i].IsFinished;
            }

            IsAllFinished = allFinished;

            if (!string.IsNullOrEmpty(mlOrder.StatusMsg))
            {
                EventLogger.Write("组合下单({0}) - {1}", mlOrder.OrderId, mlOrder.StatusMsg);
            }
        }

        public void From(string ordRef, trade.Order order)
        {
            int idx = LastOrder.Legs.FindIndex(
                l => l.InstrumentID == order.InstrumentID 
                    && l.Direction == order.Direction);
            if (idx > -1)
            {
                LastOrder.Legs[idx] = order;

                foreach (var oVm in _orders)
                {
                    if (oVm.OrderUid == OrderVM.GetOrderUid(order))
                    {
                        oVm.From(order);
                        break;
                    }
                }
            }
        }

        public void CalcProfit(MultiLegOrderVM openOrderVm)
        {
            double profit = 0;

            if (LastOrder == null) return;

            foreach (var closeOrd in LastOrder.Legs)
            {
                var openOrd = openOrderVm.LastOrder.Legs.FirstOrDefault(l => closeOrd.InstrumentID == l.InstrumentID);
                if (openOrd != null)
                {
                    if (closeOrd.Direction == trade.TradeDirectionType.SELL)
                    {
                        profit += (closeOrd.LimitPrice - openOrd.LimitPrice);
                    }
                    else
                    {
                        profit += (openOrd.LimitPrice - closeOrd.LimitPrice);
                    }
                }
            }

            Profit = profit;
        }

        private static string GetReasonDisplayText(trade.SubmitReason submitReason)
        {
            switch (submitReason)
            {
                case trade.SubmitReason.SR_AutoOpen:
                    return "开仓";
                case trade.SubmitReason.SR_Manual:
                    return "手动";
                case trade.SubmitReason.SR_StopGain:
                    return "止盈";
                case trade.SubmitReason.SR_StopLoss:
                    return "止损";
                case trade.SubmitReason.SR_AutoSwitch:
                    return "移仓";
                default:
                    return "未知";
            }
        }
    }
}
