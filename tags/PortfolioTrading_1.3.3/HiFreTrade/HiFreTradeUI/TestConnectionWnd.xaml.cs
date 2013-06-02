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
using System.ComponentModel;
using System.Diagnostics;

namespace HiFreTradeUI
{
    /// <summary>
    /// Interaction logic for TestConnectionWnd.xaml
    /// </summary>
    public partial class TestConnectionWnd : Window
    {
        private BackgroundWorker _bgWorker;

        private bool _testing = false;
        
        public TestConnectionWnd(ServerConfiguation servConfig)
        {
            this.DataContext = servConfig;
            InitializeComponent();

            _bgWorker = new BackgroundWorker();
            _bgWorker.WorkerSupportsCancellation = true;
            _bgWorker.DoWork += new DoWorkEventHandler(_bgWorker_DoWork);
            _bgWorker.RunWorkerCompleted += new RunWorkerCompletedEventHandler(_bgWorker_RunWorkerCompleted);
        }

        void _bgWorker_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            if (e.Cancelled)
            {
                Debug.WriteLine("Test cancelled");
            }
            else
            {
                Debug.WriteLine("Test finished");
            }

            this.Dispatcher.Invoke(
                new Action(
                    delegate
                    {
                        _testing = false;
                        this.btnTest.Content = "测试连接";
                        this.btnTest.IsEnabled = true;
                    }));
            
        }

        void _bgWorker_DoWork(object sender, DoWorkEventArgs e)
        {
            ServerGroup group = e.Argument as ServerGroup;
            TestServers(group.Servers, e);
        }

        
        private void btnTest_Click(object sender, RoutedEventArgs e)
        {
            if (_testing)
            {
                _bgWorker.CancelAsync();
                this.btnTest.Content = "正在停止";
                this.btnTest.IsEnabled = false;
            }
            else
            {
                _bgWorker.RunWorkerAsync(this.cmbServGroup.SelectedItem);
                _testing = true;
                this.btnTest.Content = "停止测试";
            }
        }

        private void UpdateTestStatus(ServerInfo si, ServerStatus status)
        {
            this.Dispatcher.Invoke(new Action(()=> si.Status = status));
        }

        private void TestServers(IEnumerable<ServerInfo> servers, DoWorkEventArgs e)
        {
            foreach (var si in servers)
            {
                if (_bgWorker.CancellationPending)
                {
                    e.Cancel = true;
                    return;
                }

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
