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
using HiFreTradeUI.ViewModels;
using HiFreTradeUI.Win32Invoke;

namespace HiFreTradeUI.Modules.PositionHandling
{
    /// <summary>
    /// Interaction logic for OrdersView.xaml
    /// </summary>
    [Export]
    public partial class OrdersView : UserControl
    {
        private OrdersVM ViewModel { get; set; }

        private static Gateway.OrderInfoUpdateDelegate orderInfoUpdateFunc;

        [ImportingConstructor]
        public OrdersView(OrdersVM viewModel)
        {
            ViewModel = viewModel;
            this.DataContext = ViewModel;
            InitializeComponent();
        }

        private void btnQueryOrders_Click(object sender, RoutedEventArgs e)
        {
            ViewModel.Clear();
            if (orderInfoUpdateFunc == null)
            {
                orderInfoUpdateFunc = OnOrderInfoUpdate;
                Gateway.RegOrders(orderInfoUpdateFunc);
            }

            Win32Invoke.Gateway.QueryOrders("");
            ViewModel.UpdateTime = DateTime.Now;
        }

        private void OnOrderInfoUpdate(OrderInfo orderInfo)
        {
            this.Dispatcher.Invoke(new Action(delegate
            {
                ViewModel.AddOrder(orderInfo);
            }));
        }

        private void CancelOrder_Handler(object sender, RoutedEventArgs e)
        {
            Button btnCancel = sender as Button;
            if (btnCancel != null)
            {
                OrderItem item = btnCancel.Tag as OrderItem;

                if (item != null)
                {
                    string ordRef = item.OrderRef;
                    string exchId = item.ExchangeID;
                    string ordSysId = item.OrderSysID;
                    string userId = item.UserID;
                    string symbol = item.Symbol;

                    Win32Invoke.Gateway.CancelOrder2(ordRef, exchId, ordSysId, userId, symbol);
                }
            }
        }
    }
}
