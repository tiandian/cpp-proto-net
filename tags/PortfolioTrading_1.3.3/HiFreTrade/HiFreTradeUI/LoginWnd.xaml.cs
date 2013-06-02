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
using HiFreTradeUI.ViewModels;
using HiFreTradeUI.ServerConfig;

namespace HiFreTradeUI
{
    /// <summary>
    /// Interaction logic for LoginWnd.xaml
    /// </summary>
    [Export]
    public partial class LoginWnd : Window
    {
        private LoginVM _viewModel;

        [ImportingConstructor]
        public LoginWnd(LoginVM viewModel)
        {
            _viewModel = viewModel;
            this.DataContext = _viewModel;
            InitializeComponent();
        }

        [Import]
        private ServerConfiguation ServConfig { get; set; }

        [Import]
        private QuoteVM QuoteViewModel { get; set; }

        private void btnLogin_Click(object sender, RoutedEventArgs e)
        {
            if (QuoteViewModel.MarketDataServ == null)
            {
                MessageBox.Show("请选择行情服务器", "行情", MessageBoxButton.OK, MessageBoxImage.Information);
                return;
            }
            _viewModel.Password = this.tbPassword.Password;
            _viewModel.LoginTrade(OnLoginCompleted);
        }

        private void OnLoginCompleted(bool succ)
        {
            if (succ)
            {
                HiFreTradeUI.Utils.RuntimeContext.CurrentUserID = _viewModel.AccountId;
                this.DialogResult = true;
            }
            else
            {
                MessageBox.Show("用户名或者密码错误!", "登录失败", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private void btnCancel_Click(object sender, RoutedEventArgs e)
        {
            this.DialogResult = false;
        }

        private void btnConn_Click(object sender, RoutedEventArgs e)
        {
            TestConnectionWnd connWnd = new TestConnectionWnd(ServConfig);
            connWnd.Owner = this;
            bool? res = connWnd.ShowDialog();
            if (res ?? false)
            {
                this.txtSvrName.Text = connWnd.SelectedMarketData.Name;
                QuoteViewModel.MarketDataServ = connWnd.SelectedMarketData;
            }
        }
    }
}
