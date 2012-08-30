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

namespace PortfolioTrading.Modules.Account
{
    /// <summary>
    /// Interaction logic for EditAccountDlg.xaml
    /// </summary>
    public partial class EditAccountDlg : Window
    {
        private bool _isAdding;
        public EditAccountDlg( bool adding = false )
        {
            _isAdding = adding;
            InitializeComponent();
        }

        private AccountVM _viewModel;
        public AccountVM ViewModel
        {
            get { return _viewModel; }
            set
            {
                _viewModel = value;
                this.DataContext = _viewModel;
            }
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            if (_isAdding)
                this.Title = "添加帐户";
            else
                this.Title = "编辑帐户";
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
