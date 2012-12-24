using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.ComponentModel.Composition;
using Microsoft.Practices.Prism.Events;
using PortfolioTrading.Events;
using System.Collections.ObjectModel;
using Microsoft.Practices.Prism.Commands;
using Microsoft.Practices.ServiceLocation;
using System.Diagnostics;

namespace PortfolioTrading.Modules.Portfolio
{
    /// <summary>
    /// Interaction logic for PortfolioOrdersView.xaml
    /// </summary>
    [Export]
    public partial class PortfolioOrdersView : UserControl
    {
        private MultiLegOrderRepositry _ordersRepo = new MultiLegOrderRepositry();

        public static readonly ICommand ClosePorfOrderCommand = new DelegateCommand<MultiLegOrderVM>(OnClosePorfOrder);
        public static readonly ICommand CancelPortfOrderCommand = new DelegateCommand<MultiLegOrderVM>(OnCancelPortfOrder);

        [ImportingConstructor]
        public PortfolioOrdersView(IEventAggregator evtAgg)
        {
            evtAgg.GetEvent<MultiLegOrderUpdatedEvent>().Subscribe(OnMultiLegOrderUpdated, ThreadOption.UIThread);
            evtAgg.GetEvent<IndividualOrderUpdatedEvent>().Subscribe(OnIndividualOrderUpdated, ThreadOption.UIThread);

            this.DataContext = _ordersRepo;

            InitializeComponent();
        }

        public void OnMultiLegOrderUpdated(MultiLegOrderUpdateArgs mlOrderUpdateArgs)
        {
            var rowAdded = _ordersRepo.Update(mlOrderUpdateArgs.AccountId, mlOrderUpdateArgs.MultiLegOrder);
            if (rowAdded != null)
            {
                xamDG_MlOrders.BringDataItemIntoView(rowAdded, false);
            }
        }

        public void OnIndividualOrderUpdated(OrderUpdateArgs args)
        {
            _ordersRepo.Update(args);
        }

        private static void OnClosePorfOrder(MultiLegOrderVM mlOrderVm)
        {
            IEventAggregator evtAgg = ServiceLocator.Current.GetInstance<IEventAggregator>();
            evtAgg.GetEvent<CloseMlOrderEvent>().Publish(
                new CloseMlOrderArgs 
                { 
                    AccountId = mlOrderVm.AccountId,
                    MlOrder = mlOrderVm.LastOrder,
                    LegOrderRef = ""
                });
        }

        private static void OnCancelPortfOrder(MultiLegOrderVM mlOrderVm)
        {
            IEventAggregator evtAgg = ServiceLocator.Current.GetInstance<IEventAggregator>();
            foreach (var ord in mlOrderVm.LastOrder.Legs)
            {
                if(ord.OrderStatus != trade.OrderStatusType.ALL_TRADED &&
                    ord.OrderStatus != trade.OrderStatusType.ORDER_CANCELED)
                {
                    evtAgg.GetEvent<CancelOrderEvent>().Publish(
                        new CancelOrderEventArgs
                        {
                            AccountId = mlOrderVm.AccountId,
                            OrderToCancel = ord
                        });
                }
            }
        }
    }

    public class MultiLegOrderRepositry : ObservableCollection<MultiLegOrderVM>
    {
        public MultiLegOrderVM GetMlOrderVm(string acctId, string mlOrdId)
        {
            return this.SingleOrDefault(
                    vm => vm.AccountId == acctId && vm.OrderId == mlOrdId);
        }

        public MultiLegOrderVM Update(string accountId, trade.MultiLegOrder mlOrder)
        {
            lock (this)
            {
                string ordId = mlOrder.OrderId;
                MultiLegOrderVM mlOrderVm = null;
                mlOrderVm = GetMlOrderVm(accountId, ordId);
                if (mlOrderVm != null)
                {
                    mlOrderVm.From(mlOrder);
                    if (!mlOrderVm.IsOpenOrder && mlOrderVm.IsAllFinished)
                    {
                        if (!string.IsNullOrEmpty(mlOrder.OpenOrderId))
                        {
                            var openOrderVm = GetMlOrderVm(accountId, mlOrder.OpenOrderId);
                            if(openOrderVm != null)
                                mlOrderVm.CalcProfit(openOrderVm);
                        }
                    }
                    return null;
                }
                else
                {
                    mlOrderVm = new MultiLegOrderVM(accountId);
                    mlOrderVm.From(mlOrder);
                    this.Add(mlOrderVm);
                    return mlOrderVm;
                }
            }
        }

        public void Update(OrderUpdateArgs orderUpdateArgs)
        {
            MultiLegOrderVM mlOrderVm = this.SingleOrDefault
                (vm => vm.AccountId == orderUpdateArgs.AccountId && vm.OrderId == orderUpdateArgs.MlOrderId);
            Debug.Assert(mlOrderVm != null, 
                string.Format("Cannot find parent multileg order({0}) when update individual order",
                             orderUpdateArgs.MlOrderId));
            
            if (mlOrderVm != null)
            {
                mlOrderVm.From(orderUpdateArgs.LegOrderRef, orderUpdateArgs.LegOrder);
            }
        }
    }
}
