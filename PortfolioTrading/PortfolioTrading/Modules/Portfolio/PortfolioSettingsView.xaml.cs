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

namespace PortfolioTrading.Modules.Portfolio
{
    /// <summary>
    /// Interaction logic for PortfolioSettingsView.xaml
    /// </summary>
    [Export]
    public partial class PortfolioSettingsView : UserControl
    {
        [ImportingConstructor]
        public PortfolioSettingsView(PortfSettingsVM viewModel)
        {
            InitializeComponent();
            this.DataContext = viewModel;
        }
    }
}
