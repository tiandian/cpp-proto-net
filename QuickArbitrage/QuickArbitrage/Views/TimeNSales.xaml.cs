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
using System.Collections.ObjectModel;
using QuickArbitrage.Main.ViewModel;
using QuickArbitrage.Common.Enums;

namespace QuickArbitrage.Main.Views
{
    /// <summary>
    /// Interaction logic for TimeNSales.xaml
    /// </summary>
    public partial class TimeNSales : UserControl
    {
        public TimeNSales()
        {
            InitializeComponent();
        }
    }

    class TimeNSalesViewModel : ObservableCollection<TimeNSaleItem>
    {
        public TimeNSalesViewModel()
        {
            this.Add(new TimeNSaleItem()
            {
                Code = "cu1203",
                Name = "沪铜1203",
                Action = TradeAction.ShortEntry,
                Quantity = 1,
                Price = 61240f,
                ExecuteTime = DateTime.Now.AddMinutes(-10)
            });

            this.Add(new TimeNSaleItem()
            {
                Code = "cu1204",
                Name = "沪铜1204",
                Action = TradeAction.LongEntry,
                Quantity = 1,
                Price = 61570,
                ExecuteTime = DateTime.Now.AddMinutes(-5)
            });

            this.Add(new TimeNSaleItem()
            {
                Code = "cu1203",
                Name = "沪铜1203",
                Action = TradeAction.ShortExit,
                Quantity = 1,
                Price = 61230f,
                ExecuteTime = DateTime.Now.AddMinutes(-3)
            });

            this.Add(new TimeNSaleItem()
            {
                Code = "cu1204",
                Name = "沪铜1204",
                Action = TradeAction.LongExit,
                Quantity = 1,
                Price = 61670,
                ExecuteTime = DateTime.Now.AddMinutes(-1)
            });
        }
    }
}
