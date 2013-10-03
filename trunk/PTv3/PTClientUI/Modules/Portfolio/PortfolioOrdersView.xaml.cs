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

        public MultiLegOrderRepositry OrderRepositry { get { return _ordersRepo; } }

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
                if(
                    ord.OrderSubmitStatus != PTEntity.OrderSubmitStatusType.NOT_SUBMITTED &&
                    ord.OrderStatus != PTEntity.OrderStatusType.ALL_TRADED &&
                    ord.OrderStatus != PTEntity.OrderStatusType.ORDER_CANCELED)
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
        private Dictionary<string, int> _accountPortfolioOrderCount = new Dictionary<string, int>();
        private object _syncObj = new object();

        public MultiLegOrderVM GetMlOrderVm(string acctId, string mlOrdId)
        {
            return this.SingleOrDefault(
                    vm => vm.AccountId == acctId && vm.OrderId == mlOrdId);
        }

        public MultiLegOrderVM Update(string accountId, PTEntity.MultiLegOrder mlOrder)
        {
            lock (_syncObj)
            {
                string ordId = mlOrder.OrderId;
                MultiLegOrderVM mlOrderVm = null;
                mlOrderVm = GetMlOrderVm(accountId, ordId);
                if (mlOrderVm != null)
                {
                    mlOrderVm.From(mlOrder);

                    if (mlOrderVm.IsAllFinished)
                    {
                        if (mlOrderVm.IsPortfolio)
                        {
                            if (!mlOrderVm.IsOpenOrder)
                            {
                                if (!string.IsNullOrEmpty(mlOrder.OpenOrderId))
                                {
                                    var openOrderVm = GetMlOrderVm(accountId, mlOrder.OpenOrderId);
                                    if (openOrderVm != null)
                                        mlOrderVm.CalcProfit(openOrderVm);
                                }
                            }
                        }
                        else // the first leg is to open position, and the second leg is to close position
                        {
                            mlOrderVm.CalcProfit();
                        }
                    }
                    
                    return null;
                }
                else
                {
                    string accountPortfKey = BuildAcctPortfKey(accountId, mlOrder.PortfolioId);
                    if (_accountPortfolioOrderCount.ContainsKey(accountPortfKey))
                    {
                        _accountPortfolioOrderCount[accountPortfKey] += 1;
                    }
                    else
                    {
                        _accountPortfolioOrderCount.Add(accountPortfKey, 1);
                    }

                    mlOrderVm = new MultiLegOrderVM(accountId);
                    mlOrderVm.From(mlOrder);
                    this.Add(mlOrderVm);
                    return mlOrderVm;
                }
            }
        }

        private static string BuildAcctPortfKey(string accountId, string portfolioId)
        {
            return string.Format("{0}-{1}", accountId, portfolioId);
        }

        public void Update(OrderUpdateArgs orderUpdateArgs)
        {
            lock (_syncObj)
            {
                MultiLegOrderVM mlOrderVm = GetMlOrderVm(orderUpdateArgs.AccountId, orderUpdateArgs.MlOrderId);
                Debug.Assert(mlOrderVm != null,
                    string.Format("Cannot find parent multileg order({0}) when update individual order",
                                 orderUpdateArgs.MlOrderId));

                if (mlOrderVm != null)
                {
                    mlOrderVm.From(orderUpdateArgs.LegOrderRef, orderUpdateArgs.LegOrder);
                }
            }
        }

        public int GetAccountOrderCount(string accountId, string portfolioId)
        {
            lock (_syncObj)
            {
                string acctPortfKey = BuildAcctPortfKey(accountId, portfolioId);
                return _accountPortfolioOrderCount.ContainsKey(acctPortfKey) ? _accountPortfolioOrderCount[acctPortfKey] : 0;
            }
        }
    }
}
