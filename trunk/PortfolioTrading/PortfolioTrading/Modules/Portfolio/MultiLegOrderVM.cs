using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.Prism.ViewModel;
using System.Collections.ObjectModel;

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


        public IEnumerable<OrderVM> Legs
        {
            get { return _orders; }
        }

        public void From(trade.MultiLegOrder mlOrder)
        {
            LastOrder = mlOrder;

            OrderId = mlOrder.OrderId;
            PortfolioId = mlOrder.PortfolioId;
            Quantity = mlOrder.Quantity;
            IsOpenOrder = mlOrder.OrderId == mlOrder.OpenOrderId;

            bool allFinished = false;
            for (int i = 0; i < mlOrder.Legs.Count; ++i )
            {
                if (_orders.Count == i)
                    _orders.Add(new OrderVM());

                var legOrder = mlOrder.Legs[i];
                _orders[i].From(legOrder);
                allFinished = _orders[i].IsFinished;
            }

            IsAllFinished = allFinished;
        }

        public void From(string ordRef, trade.Order order)
        {
            int idx = LastOrder.Legs.FindIndex(l => l.OrderRef == ordRef);
            if (idx > -1)
            {
                LastOrder.Legs[idx] = order;

                foreach (var oVm in _orders)
                {
                    if (oVm.OrderRef == ordRef)
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
    }
}
