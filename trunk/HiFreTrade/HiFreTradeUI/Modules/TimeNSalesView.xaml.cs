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

namespace HiFreTradeUI.Modules
{
    /// <summary>
    /// Interaction logic for TimeNSalesView.xaml
    /// </summary>
    [Export]
    public partial class TimeNSalesView : UserControl
    {
        private TimeNSalesVM _viewModel;

        [ImportingConstructor]
        public TimeNSalesView(TimeNSalesVM viewModel)
        {
            _viewModel = viewModel;
            this.DataContext = _viewModel;
            InitializeComponent();
        }
    }
}
