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

        #region MaxOpenPerStart
        private int _maxOpenPerStart;

        public int MaxOpenPerStart
        {
            get { return _maxOpenPerStart; }
            set
            {
                if (_maxOpenPerStart != value)
                {
                    _maxOpenPerStart = value;
                    RaisePropertyChanged("MaxOpenPerStart");
                }
            }
        }
        #endregion

        #region MaxCancel
        private int _maxCancel;

        public int MaxCancel
        {
            get { return _maxCancel; }
            set
            {
                if (_maxCancel != value)
                {
                    _maxCancel = value;
                    RaisePropertyChanged("MaxCancel");
                }
            }
        }
        #endregion

        #region TotalOpenLimit
        private int _totalOpenLimit;

        public int TotalOpenLimit
        {
            get { return _totalOpenLimit; }
            set
            {
                if (_totalOpenLimit != value)
                {
                    _totalOpenLimit = value;
                    RaisePropertyChanged("TotalOpenLimit");
                }
            }
        }
        #endregion

        #region EndTimePointsExpr
        private string _endTimePointsExpr;

        public string EndTimePointsExpr
        {
            get { return _endTimePointsExpr; }
            set
            {
                if (_endTimePointsExpr != value)
                {
                    _endTimePointsExpr = value;
                    RaisePropertyChanged("EndTimePointsExpr");
                }
            }
        }
        #endregion

        public string[] getEndTimePoints()
        {
            List<string> ret = new List<string>();
            if (!string.IsNullOrEmpty(_endTimePointsExpr))
            {
                string[] tps = _endTimePointsExpr.Split(',');
                foreach (string s in tps)
                {
                    if (!string.IsNullOrWhiteSpace(s))
                        ret.Add(s.Trim());
                }
            }

            return ret.ToArray(); ;
        }
    }
}
