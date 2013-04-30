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
using Microsoft.Practices.Prism.ViewModel;

namespace PortfolioTrading.Modules.Portfolio
{
    /// <summary>
    /// Interaction logic for ModifyMaxQtyDlg.xaml
    /// </summary>
    public partial class ModifyMaxQtyDlg : Window
    {
        public ModifyMaxQtyDlg(ModifyQtyViewModel viewModel)
        {
            this.DataContext = viewModel;
            InitializeComponent();
        }

        private void btnOK_Click(object sender, RoutedEventArgs e)
        {
            this.DialogResult = true;
        }

        private void btnCancel_Click(object sender, RoutedEventArgs e)
        {
            this.DialogResult = false;
        }
    }

    public class ModifyQtyViewModel : NotificationObject
    {
        public string PortfolioId { get; set; }

        #region OnceQuantity
        private int _onceQty;

        public int OnceQuantity
        {
            get { return _onceQty; }
            set
            {
                if (_onceQty != value)
                {
                    _onceQty = value;
                    RaisePropertyChanged("OnceQuantity");
                }
            }
        }
        #endregion

        #region MaxQuantity
        private int _maxQty;

        public int MaxQuantity
        {
            get { return _maxQty; }
            set
            {
                if (_maxQty != value)
                {
                    _maxQty = value;
                    RaisePropertyChanged("MaxQuantity");
                }
            }
        }
        #endregion

    }
}
