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
using PortfolioTrading.Modules.Account;

namespace PortfolioTrading.Modules.Portfolio
{
    /// <summary>
    /// Interaction logic for PortfoliosView.xaml
    /// </summary>
    [Export]
    public partial class PortfoliosView : UserControl
    {
        [ImportingConstructor]
        public PortfoliosView(IEventAggregator evtAgg)
        {
            evtAgg.GetEvent<AccountSelectedEvent>().Subscribe(OnAccountSelected, ThreadOption.UIThread);
            InitializeComponent();
        }

        public void OnAccountSelected(AccountVM accountVm)
        {
            this.DataContext = accountVm;
        }
    }
}
