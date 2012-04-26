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

namespace AutoTrade.MainFrame.Modules.Account
{
    /// <summary>
    /// Interaction logic for AccountTree.xaml
    /// </summary>
    [Export]
    public partial class AccountTree : UserControl
    {
        public AccountTree()
        {
            InitializeComponent();
        }


        private AccountManager _accountMgr;

        [Import]
        public AccountManager AccountManager
        {
            get { return _accountMgr; }
            set 
            { 
                _accountMgr = value;
                this.DataContext = _accountMgr;
            }
        }

        private void xamDataTreeAccounts_SelectedNodesCollectionChanged(object sender, Infragistics.Controls.Menus.NodeSelectionEventArgs e)
        {
            var nodes = e.CurrentSelectedNodes;
            if (nodes.Count > 0)
            {
                AccountInfo acct = nodes[0].Data as AccountInfo;
                AccountManager.SelectedAccount = acct;
            }
        }

    }
}
