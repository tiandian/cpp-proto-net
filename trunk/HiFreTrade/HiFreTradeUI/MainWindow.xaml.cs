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
using HiFreTradeUI.Win32Invoke;
using System.Diagnostics;

namespace HiFreTradeUI
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

        private void btnTest_Click(object sender, RoutedEventArgs e)
        {
            int ret = Gateway.TestCall(12, 32);
            Debug.WriteLine("Return value is {0}", ret);

            bool connected = Gateway.ConnectMarketAgent("0240", "23412434", "888888");
            Debug.WriteLine("Connected {0}", connected);
        }
    }
}
