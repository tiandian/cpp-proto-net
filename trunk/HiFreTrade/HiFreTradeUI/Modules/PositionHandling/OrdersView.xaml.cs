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

namespace HiFreTradeUI.Modules.PositionHandling
{
    /// <summary>
    /// Interaction logic for OrdersView.xaml
    /// </summary>
    [Export]
    public partial class OrdersView : UserControl
    {
        public OrdersView()
        {
            InitializeComponent();
        }
    }
}
