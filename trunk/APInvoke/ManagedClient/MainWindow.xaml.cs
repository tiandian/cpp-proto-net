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

        public MainWindow()
        {
            _client = new Client();

            InitializeComponent();
        }

        private void btnConnect_Click(object sender, RoutedEventArgs e)
        {
            _client.ConnectAsync("127.0.0.1", 16168, 
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
            Process proc = Process.Start("NativeServer.exe", "16168");
            if (proc != null)
            {
                proc.EnableRaisingEvents = true;
                proc.Exited += new EventHandler(proc_Exited);
            }
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
    }
}
