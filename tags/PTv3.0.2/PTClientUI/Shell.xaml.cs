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
using System.Windows.Shapes;
using System.ComponentModel.Composition;
using Microsoft.Practices.Prism.Events;
using PortfolioTrading.Events;
using PortfolioTrading.Utils;

namespace PortfolioTrading
{
    /// <summary>
    /// Interaction logic for Shell.xaml
    /// </summary>
    [Export]
    public partial class Shell : Window
    {
        private IEventAggregator EventAggregator { get; set; }

        [ImportingConstructor]
        public Shell(IEventAggregator evtAgg)
        {
            EventAggregator = evtAgg;
            InitializeComponent();
        }
       
        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            DispatcherHelper.Init(this.Dispatcher);
        }

        private void Window_Closed(object sender, EventArgs e)
        {
            EventAggregator.GetEvent<AppShutDown>().Publish("");
        }

    }
}
