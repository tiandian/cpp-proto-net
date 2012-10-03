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
using System.Xml.Linq;
using Infragistics.DragDrop;
using Infragistics.Controls.Menus;

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

        private void xamDataTreeAccount_ActiveNodeChanged(object sender, Infragistics.Controls.Menus.ActiveNodeChangedEventArgs e)
        {
            AccountVM acctVm = e.NewActiveTreeNode.Data as AccountVM;
            
            if (acctVm != null)
                EventAggregator.GetEvent<AccountSelectedEvent>().Publish(acctVm);
        }

        private void xamDataTreeAccount_NodeDragEnd(object sender, Infragistics.DragDrop.DragDropEventArgs e)
        {
            if (e.OperationType != null && e.OperationType == OperationType.Copy)
            {
                XamDataTreeNode origNode = e.Data as XamDataTreeNode;
                if (origNode == null) return;
                PortfolioVM porfVm = origNode.Data as PortfolioVM;
                if (porfVm == null) return;

                XamDataTreeNodeControl treeNodeCtrl = e.DropTarget as XamDataTreeNodeControl;
                if (treeNodeCtrl == null) return;
                AccountVM acctVm = treeNodeCtrl.Node.Data as AccountVM;
                if (acctVm == null) return;

                XElement portfElem = porfVm.Persist();
                var cloned = PortfolioVM.Load(acctVm, portfElem);
                acctVm.AddPorfolio(cloned);

                ViewModel.Persist();
            }
        }
    }
}
