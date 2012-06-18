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
using HiFreTradeUI.Win32Invoke;
using HiFreTradeUI.ServerConfig;

namespace HiFreTradeUI
{
    /// <summary>
    /// Interaction logic for TestConnectionWnd.xaml
    /// </summary>
    public partial class TestConnectionWnd : Window
    {
        public TestConnectionWnd(ServerConfiguation servConfig)
        {
            this.DataContext = servConfig;
            InitializeComponent();
        }

        private void btnTest_Click(object sender, RoutedEventArgs e)
        {
            ServerGroup group = this.cmbServGroup.SelectedItem as ServerGroup;

            Action<IEnumerable<ServerInfo>> testFunc = TestServers;
            testFunc.BeginInvoke(group.Servers, 
                delegate(IAsyncResult ar)
                {
                    testFunc.EndInvoke(ar);
                }, null);
        }

        private void UpdateTestStatus(ServerInfo si, ServerStatus status)
        {
            this.Dispatcher.Invoke(new Action(()=> si.Status = status));
        }

        private void TestServers(IEnumerable<ServerInfo> servers)
        {
            foreach (var si in servers)
            {
                UpdateTestStatus(si, ServerStatus.Testing);
                int delay = TestAddress(si.Address);
                if(delay > 0)
                    UpdateTestStatus(si, ServerStatus.Available);
                else
                    UpdateTestStatus(si, ServerStatus.Failed);
            }
        }

        private int TestAddress(string address)
        {
            return Gateway.TestMarketAddress(address);
        }

        public ServerInfo SelectedMarketData { get; private set; }

        private void btnDetermine_Click(object sender, RoutedEventArgs e)
        {
            ServerInfo si = this.lbServers.SelectedItem as ServerInfo;
            if (si == null)
            {
                MessageBox.Show("请选择一个行情服务器", "连接行情", MessageBoxButton.OK, MessageBoxImage.Information);
                return;
            }

            SelectedMarketData = si;
            this.DialogResult = true;
            this.Close();
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            if (this.cmbServGroup.Items.Count > 0)
                this.cmbServGroup.SelectedIndex = 0;
        }
    }
}
