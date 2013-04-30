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

namespace PortfolioTrading.Modules.Account
{
    /// <summary>
    /// Interaction logic for ServerAddrConfigDlg.xaml
    /// </summary>
    public partial class ServerAddrConfigDlg : Window
    {
        public ServerAddrConfigDlg(ServerAddressRepoVM viewModel)
        {
            InitializeComponent();
            this.DataContext = viewModel;
        }

        private void btnOK_Click(object sender, RoutedEventArgs e)
        {
            this.DialogResult = true;
        }

        private void btnCancel_Click(object sender, RoutedEventArgs e)
        {
            this.DialogResult = false;
        }
    }
}
