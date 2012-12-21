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
    /// Interaction logic for ChangeSpecifiedQtyDlg.xaml
    /// </summary>
    public partial class ChangeSpecifiedQtyDlg : Window
    {
        public ChangeSpecifiedQtyDlg(SpecifyQuantityViewModel viewModel)
        {
            DataContext = viewModel;
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

    public class SpecifyQuantityViewModel : NotificationObject
    {
        public enum SpecifyQtyMode { Open, Close, Move }

        private readonly SpecifyQtyMode _mode; 

        public SpecifyQuantityViewModel(SpecifyQtyMode mode)
        {
            _mode = mode;
        }

        public string ModeText
        {
            get
            {
                switch (_mode)
                {
                    case SpecifyQtyMode.Open:
                        return "开仓";
                    case SpecifyQtyMode.Close:
                        return "平仓";
                    case SpecifyQtyMode.Move:
                        return "移仓";
                    default:
                        return "未知";
                }
            }
        }

        public string ModeTitle
        {
            get { return string.Format("输入{0}数量", ModeText); }
        }

        #region Quantity
        private int _qty;

        public int Quantity
        {
            get { return _qty; }
            set
            {
                if (_qty != value)
                {
                    _qty = value;
                    RaisePropertyChanged("Quantity");
                }
            }
        }
        #endregion

    }
}
