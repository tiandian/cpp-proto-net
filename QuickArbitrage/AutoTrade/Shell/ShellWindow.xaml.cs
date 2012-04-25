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

namespace AutoTrade.MainFrame
{
    /// <summary>
    /// Interaction logic for ShellWindow.xaml
    /// </summary>
    [Export]
    partial class ShellWindow : Window
    {
        private ShellViewModel _viewModel;

        [ImportingConstructor]
        internal ShellWindow(ShellViewModel viewModel)
        {
            _viewModel = viewModel;
            this.DataContext = _viewModel;
            _viewModel.GetOwner = () => this;

            InitializeComponent();
        }

        private string _savedLayout;

        private void btnSaveLayout_Click(object sender, RoutedEventArgs e)
        {
            _savedLayout = dockManager.SaveLayout();
        }

        private void btnLoadLayout_Click(object sender, RoutedEventArgs e)
        {
            dockManager.LoadLayout(_savedLayout);
        }
    }
}
