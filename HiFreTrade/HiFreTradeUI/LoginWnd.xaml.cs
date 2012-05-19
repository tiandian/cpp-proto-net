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

        private void btnLogin_Click(object sender, RoutedEventArgs e)
        {
            _viewModel.Password = this.tbPassword.Password;
            _viewModel.LoginTrade(OnLoginCompleted);
        }

        private void OnLoginCompleted(bool succ)
        {
            if (succ)
            {
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
    }
}
