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
using PortfolioTrading.Modules.Account;
using Microsoft.Practices.Prism.Events;
using PortfolioTrading.Events;

namespace PortfolioTrading.Modules.Account
{
    /// <summary>
    /// Interaction logic for AccountMgrView.xaml
    /// </summary>
    [Export]
    public partial class AccountMgrView : UserControl
    {
        private AccountMgrVM ViewModel { get; set; }
        private IEventAggregator EventAggregator { get; set; }
        [ImportingConstructor]
        public AccountMgrView(AccountMgrVM viewModel, IEventAggregator evtAgg)
        {
            ViewModel = viewModel;
            this.DataContext = ViewModel;

            EventAggregator = evtAgg;
            InitializeComponent();
        }

        private void xamDataTreeAccount_SelectedNodesCollectionChanged(object sender, Infragistics.Controls.Menus.NodeSelectionEventArgs e)
        {
            if (e.CurrentSelectedNodes.Count > 0)
            {
                AccountVM acctVm = e.CurrentSelectedNodes[0].Data as AccountVM;
                if(acctVm != null)
                    EventAggregator.GetEvent<AccountSelectedEvent>().Publish(acctVm);
            }
        }
    }
}
