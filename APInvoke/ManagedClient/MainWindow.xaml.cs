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
using System.Diagnostics;

namespace ManagedClient
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private Client _client;
        private Process _servProc;

        private const int ServerPort = 16168;

        public MainWindow()
        {
            _client = new Client();
            _client.OnError += new Action<string>(_client_OnError);
            
            InitializeComponent();

            InitServProc();
        }

        void _client_OnError(string errorMsg)
        {
            Debug.WriteLine(string.Format("Client encounter error: {0}", errorMsg));
        }

        private void InitServProc()
        {
            _servProc = new Process();
            _servProc.StartInfo.FileName = "NativeServer.exe";
            _servProc.StartInfo.Arguments = ServerPort.ToString();

            _servProc.EnableRaisingEvents = true;
            _servProc.Exited += new EventHandler(proc_Exited);
        }

        private void btnConnect_Click(object sender, RoutedEventArgs e)
        {
            _client.ConnectAsync("127.0.0.1", ServerPort, 
                delegate(bool succ, string err)
                {
                    string msg = string.Format("{0} to connect server", 
                        succ ? "Succeeded" : "Failed");
                });
        }

        private void btnSend_Click(object sender, RoutedEventArgs e)
        {

        }

        private void btnLaunch_Click(object sender, RoutedEventArgs e)
        {
            if (chkHideConsole.IsChecked ?? false)
            {
                _servProc.StartInfo.UseShellExecute = false;
                _servProc.StartInfo.CreateNoWindow = true;
                _servProc.StartInfo.RedirectStandardInput = true;
            }
            else
            {
                _servProc.StartInfo.UseShellExecute = true;
                _servProc.StartInfo.CreateNoWindow = false;
                _servProc.StartInfo.RedirectStandardInput = false;
            }

            _servProc.Start();
            
        }

        void proc_Exited(object sender, EventArgs e)
        {
            PromptMessage("NativeServer has exist");
        }

        private void PromptMessage(string msg)
        {
            this.Dispatcher.Invoke(
                new Action(delegate
                    {
                        MessageBox.Show(msg);
                    }));
        }

        private void btnShutdown_Click(object sender, RoutedEventArgs e)
        {
            // let server process quit
            if (_servProc.StartInfo.RedirectStandardInput)
                _servProc.StandardInput.WriteLine("q");
            else
                _servProc.Kill();
        }

        private void btnSyncConnect_Click(object sender, RoutedEventArgs e)
        {
            bool succ = _client.Connect("127.0.0.1", ServerPort);
            MessageBox.Show(succ ? "Success" : "Failure");
        }

        private void btnDisconnect_Click(object sender, RoutedEventArgs e)
        {
            _client.Disconnect();
        }
    }
}
