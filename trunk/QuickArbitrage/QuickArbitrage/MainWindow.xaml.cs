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
using QuickArbitrage.Common;
using QuickArbitrage.Main.Dialogs;

namespace QuickArbitrage
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            LogManager.Logger.Info("Test logging");
        }

        private void btnNewPortfolio_Click(object sender, RoutedEventArgs e)
        {
            NewPortfolioDlg dlg = new NewPortfolioDlg();
            dlg.Owner = this;
            dlg.ShowDialog();
        }
    }
}
