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
    /// Interaction logic for AccountDetail.xaml
    /// </summary>
    [Export]
    public partial class AccountDetail : UserControl
    {
        private AccountManager _acctMgr;

        [Import]
        public AccountManager AccountManager
        {
            get { return _acctMgr; }
            set 
            { 
                _acctMgr = value;
                this.DataContext = _acctMgr;
            }
        }

        public AccountDetail()
        {
            InitializeComponent();
        }
    }
}
