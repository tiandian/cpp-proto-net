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

        [ImportingConstructor]
        public PortfolioOrdersView(IEventAggregator evtAgg)
        {
            evtAgg.GetEvent<MultiLegOrderUpdatedEvent>().Subscribe(OnMultiLegOrderUpdated, ThreadOption.UIThread);

            this.DataContext = _ordersRepo;

            InitializeComponent();
        }

        public void OnMultiLegOrderUpdated(trade.MultiLegOrder mlOrder)
        {
            _ordersRepo.Update(mlOrder);
        }

        private static void OnClosePorfOrder(MultiLegOrderVM mlOrderVm)
        {
            IEventAggregator evtAgg = ServiceLocator.Current.GetInstance<IEventAggregator>();
            evtAgg.GetEvent<CloseMlOrderEvent>().Publish(
                new CloseMlOrderArgs { MlOrder = mlOrderVm.LastOrder });
        }
    }

    public class MultiLegOrderRepositry : ObservableCollection<MultiLegOrderVM>
    {
        public void Update(trade.MultiLegOrder mlOrder)
        {
            lock (this)
            {
                string ordId = mlOrder.OrderId;
                MultiLegOrderVM mlOrderVm = null;
                mlOrderVm = this.SingleOrDefault(vm => vm.OrderId == ordId);
                if (mlOrderVm != null)
                {
                    mlOrderVm.From(mlOrder);
                }
                else
                {
                    mlOrderVm = new MultiLegOrderVM();
                    mlOrderVm.From(mlOrder);
                    this.Add(mlOrderVm);
                }
            }
        }
    }
}
