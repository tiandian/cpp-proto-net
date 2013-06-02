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
using AutoTrade.MainFrame.Utils;
using Microsoft.Practices.Prism.ViewModel;
using QuickArbitrage.Common;

namespace AutoTrade.MainFrame.Modules.Portfolio
{
    /// <summary>
    /// Interaction logic for AddPortfolioDlg.xaml
    /// </summary>
    public partial class AddEditPortfolioDlg : Window
    {
        public AddEditPortfolioDlg(IFuturesRepositry futuresList, PortfolioItem portfolioItem)
        {
            if (portfolioItem.Legs.Count() == 2)
                this.Title = "编辑组合";

            this.DataContext = new EditPortfolioViewModel(futuresList.AllFutures, portfolioItem);

            InitializeComponent();
        }

        private void btnOK_Click(object sender, RoutedEventArgs e)
        {
            DialogResult = true;
            this.Close();
        }

        private void btnCancel_Click(object sender, RoutedEventArgs e)
        {
            DialogResult = false;
            this.Close();
        }

        public class EditPortfolioViewModel
        {
            public EditPortfolioViewModel(IEnumerable<Future> futures, PortfolioItem portfolioItem)
            {
                FutureList = futures;
                Portfolio = portfolioItem;

                if (Portfolio.Legs.Count() == 0)
                {
                    LegItem leg = Portfolio.CreateLeg();
                    Portfolio.AddLeg(leg);
                    leg = Portfolio.CreateLeg();
                    Portfolio.AddLeg(leg);
                }
            }

            public IEnumerable<Future> FutureList { get; private set; }

            public PortfolioItem Portfolio { get; private set; }
        }
    }
}
