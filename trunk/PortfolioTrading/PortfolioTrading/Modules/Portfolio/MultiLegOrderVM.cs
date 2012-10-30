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

            for (int i = 0; i < mlOrder.Legs.Count; ++i )
            {
                if (_orders.Count == i)
                    _orders.Add(new OrderVM());

                var legOrder = mlOrder.Legs[i];
                _orders[i].From(legOrder);
            }
        }

        public void From(string ordRef, trade.Order order)
        {
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
}
