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

namespace PortfolioTrading.Modules.Portfolio
{
    /// <summary>
    /// Interaction logic for EditStdDiffDlg.xaml
    /// </summary>
    public partial class EditStdDiffDlg : Window
    {
        public EditStdDiffDlg()
        {
            InitializeComponent();
        }

        public double StdDiffVal { get; set; }

        private void btnOK_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                double inteval = double.Parse(txtInterval.Text);
                double barHeight = double.Parse(txtBarHeight.Text);
                double macdVal = double.Parse(txtMacdVal.Text);
                
                StdDiffVal = macdVal * inteval / barHeight;
                StdDiffVal = Math.Round(StdDiffVal, 2, MidpointRounding.AwayFromZero);
                this.DialogResult = true;
                this.Close();
            }
            catch (System.Exception ex)
            {
            	MessageBox.Show(ex.Message);
            }
        }

        private void btnCancel_Click(object sender, RoutedEventArgs e)
        {
            this.DialogResult = false;
            this.Close();
        }
    }
}
