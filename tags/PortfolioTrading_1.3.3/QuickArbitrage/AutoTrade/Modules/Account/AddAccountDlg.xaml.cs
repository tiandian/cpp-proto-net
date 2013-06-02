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
using Microsoft.Practices.Prism.ViewModel;

namespace AutoTrade.MainFrame.Modules.Account
{
    /// <summary>
    /// Interaction logic for AddAccount.xaml
    /// </summary>
    public partial class AddAccountDlg : Window
    {
        public AddAccountDlg(AccountInfo account)
        {
            this.DataContext = account;
            InitializeComponent();

            if (!string.IsNullOrEmpty(account.AccountID))
            {
                this.Title = "修改帐号";
            }
        }

        private void btnOK_Click(object sender, RoutedEventArgs e)
        {
            DialogResult = true;
            this.Close();
        }

        private void btnCancel_Click(object sender, RoutedEventArgs e)
        {
            DialogResult = false;
            this.Close();
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            this.tbAccountId.Focus();
        }
    }
}
