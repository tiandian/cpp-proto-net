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
using Microsoft.Practices.Prism.Events;
using PortfolioTrading.Events;

namespace PortfolioTrading.Modules.Account
{
    /// <summary>
    /// Interaction logic for AccountInfoView.xaml
    /// </summary>
    [Export]
    public partial class AccountInfoView : UserControl
    {
        [ImportingConstructor]
        public AccountInfoView(IEventAggregator eventAgg)
        {
            InitializeComponent();

            eventAgg.GetEvent<AccountSelectedEvent>().Subscribe(OnAccountSelected);
        }

        private void OnAccountSelected(AccountVM accountVm)
        {
            this.DataContext = accountVm.AccountInfo;
        }
    }
}
