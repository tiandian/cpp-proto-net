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
    /// Interaction logic for EditPortfolioDlg.xaml
    /// </summary>
    public partial class EditPortfolioDlg : Window
    {
        public EditPortfolioDlg()
        {
            InitializeComponent();
        }

        private void btnOK_Click(object sender, RoutedEventArgs e)
        {
            DialogResult = true;
        }

        private void btnCancel_Click(object sender, RoutedEventArgs e)
        {
            DialogResult = false;
        }

        public PortfolioVM Portfolio
        {
            get;
            set;
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {

        }
    }
}
