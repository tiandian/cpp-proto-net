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
using PortfolioTrading.ViewModels;

namespace PortfolioTrading.Modules.Account
{
    /// <summary>
    /// Interaction logic for AccountMgrView.xaml
    /// </summary>
    [Export]
    public partial class AccountMgrView : UserControl
    {
        private AccountMgrVM ViewModel { get; set; }

        [ImportingConstructor]
        public AccountMgrView(AccountMgrVM viewModel)
        {
            ViewModel = viewModel;
            this.DataContext = ViewModel;

            InitializeComponent();
        }
    }
}
