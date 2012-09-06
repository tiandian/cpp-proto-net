using System;
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

            for (int i = 0; i < mlOrder.Legs.Count; ++i )
            {
                if (_orders.Count == i)
                    _orders.Add(new OrderVM());

                var legOrder = mlOrder.Legs[i];
                _orders[i].From(legOrder);

                if (!legOrder.SubmitSuccess)
                {
                    var o = _orders[i];
                    EventLogger.Write("{0} {1} 被拒绝 -({2})", o.Direction, o.Symbol, legOrder.SubmitError);
                }
                else if (legOrder.OrderSubmitStatus > trade.OrderSubmitStatusType.ACCEPTED)
                {
                    var o = _orders[i];
                    EventLogger.Write("{0} {1} 被拒绝 -({2})", o.Direction, o.Symbol, legOrder.StatusMsg);
                }
            }
        }
    }
}
