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
    /// Interaction logic for PositionView.xaml
    /// </summary>
    [Export]
    public partial class PositionView : UserControl
    {
        private PositionDetailVM ViewModel { get; set; }

        private static Gateway.PositionDetailUpdateDelegate positionDetailUpdateFunc;

        [ImportingConstructor]
        public PositionView(PositionDetailVM viewModel)
        {
            ViewModel = viewModel;
            this.DataContext = ViewModel;
            InitializeComponent();
        }

        private void btnQueryPosition_Click(object sender, RoutedEventArgs e)
        {
            ViewModel.Clear();
            if (positionDetailUpdateFunc == null)
            {
                positionDetailUpdateFunc = OnPositionDetailUpdate;
                Gateway.RegPositionDetail(positionDetailUpdateFunc);
            }

            Win32Invoke.Gateway.QueryPositionDetail("");
            ViewModel.UpdateTime = DateTime.Now;
        }

        private void ClosePosition_Handler(object sender, RoutedEventArgs e)
        {
            Button btnClose = sender as Button;
            if (btnClose != null)
            {
                PositionDetailItem item = btnClose.Tag as PositionDetailItem;
                string symbol = item.Symbol;
                int longshort = GetDirection(item.Direction);
                int quantity = item.Volume - item.CloseVolume;
                bool closeYesterday = item.OpenDate < item.TradingDay;
                Win32Invoke.Gateway.ForceClose(symbol, longshort, quantity, closeYesterday);
            }
        }

        private static int GetDirection(string direction)
        {
            switch (direction)
            {
                case "空头":
                    return 1;
                case "多头":
                    return 2;
                default:
                    return 0; // something wrong
            }
        }

        private void OnPositionDetailUpdate(PositionDetail positionDetail)
        {
            this.Dispatcher.Invoke(new Action(delegate
                {
                    ViewModel.AddDetailItem(positionDetail);
                }));
        }
    }
}
