﻿using System;
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
using System.ComponentModel.Composition;
using HiFreTradeUI.ViewModels;

namespace HiFreTradeUI.Modules
{
    /// <summary>
    /// Interaction logic for PositionView.xaml
    /// </summary>
    [Export]
    public partial class AccountView : UserControl
    {
        private AccountVM ViewModel {get;set;}

        [ImportingConstructor]
        public AccountView(AccountVM viewModel)
        {
            ViewModel = viewModel;
            this.DataContext = ViewModel;
            ViewModel.UIDispatcher = this.Dispatcher;

            InitializeComponent();
        }

        private void btnQueryAccount_Click(object sender, RoutedEventArgs e)
        {
            ViewModel.QueryAccount();
        }

        private void UserControl_Loaded(object sender, RoutedEventArgs e)
        {
            ViewModel.QueryAccount();
        }
    }
}
