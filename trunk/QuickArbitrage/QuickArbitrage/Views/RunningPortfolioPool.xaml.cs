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
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Collections.ObjectModel;
using QuickArbitrage.Main.ViewModel;
using QuickArbitrage.Common.Enums;
using System.ComponentModel;
using System.Windows.Threading;
using Infragistics.Windows.Controls;

namespace QuickArbitrage.Main.Views
{
    /// <summary>
    /// Interaction logic for RunningPortfolioPool.xaml
    /// </summary>
    public partial class RunningPortfolioPool : UserControl
    {
        private RunningPortfoliosViewModel _runningPortfolios;

        public RunningPortfolioPool()
        {
            InitializeComponent();

            _runningPortfolios = this.FindResource("portfolios") as RunningPortfoliosViewModel;
        }

        private void tabTradingType_SelectionChanged(object sender, Controls.TabSelectionChangedEventArgs e)
        {
            TradingTypeOptionItem item = e.SelectedItem as TradingTypeOptionItem;
            this.dgFilter.Conditions.Clear();
            if(item.TradingType != TradingType.Unknown)
            {
                this.dgFilter.Conditions.Add(
                    new Infragistics.Windows.Controls.ComparisonCondition(
                        ComparisonOperator.Equals, item.TradingType));
            }
        }

        private void UserControl_Loaded(object sender, RoutedEventArgs e)
        {
 
        }

        private void EditPortfolioButtonClicked(object sender, RoutedEventArgs e)
        {
            //QuickArbitrage.Connection.TransferTest.StreamFileTest.WriteCustomer("e:\\cus.bin");
            QuickArbitrage.Connection.ConnectionBase conn = new QuickArbitrage.Connection.ConnectionBase();
            conn.Connect();
        }

        private void DeletePortfolioButtonClicked(object sender, RoutedEventArgs e)
        {
//             QuickArbitrage.Connection.TransferTest.Customer customer =
//                 QuickArbitrage.Connection.TransferTest.StreamFileTest.ReadCustomer("e:\\cpp.bin");

        }
    }

    class TradingTypeOptionItem
    {
        public TradingType TradingType { get; set; }
        public string DisplayName { get; set; }
    }

    class TradingTypeOptions : List<TradingTypeOptionItem>
    {
        public TradingTypeOptions()
        {
            this.Add(new TradingTypeOptionItem() { TradingType = TradingType.Manual, DisplayName = "手动" });
            this.Add(new TradingTypeOptionItem() { TradingType = TradingType.Auto, DisplayName = "自动" });
            this.Add(new TradingTypeOptionItem() { TradingType = TradingType.Unknown, DisplayName = "全部" });
        }
    }

    class RunningTypeItems : List<TradingType>
    {
        public RunningTypeItems()
        {
            this.Add(TradingType.Manual);
            this.Add(TradingType.Auto);
        }
    }

    class RunningPortfoliosViewModel : ObservableCollection<RunningPortfolioItem>
    {
        public RunningPortfoliosViewModel()
        {
            EquityItem eqA = new EquityItem()
            {
                Code = "cu1203",
                Name = "沪铜1203",
                Side = Side.Short,
                PrevClose = 60990,
                Cost = 61240,
                Last = 60980
            };

            EquityItem eqB = new EquityItem()
            {
                Code = "cu1204",
                Name = "沪铜1204",
                Side = Side.Long,
                PrevClose = 61280,
                Cost = 61570,
                Last = 61290
            };

            RunningPortfolioItem portfolio = new RunningPortfolioItem();
            portfolio.Equities.Add(eqA);
            portfolio.Equities.Add(eqB);

            portfolio.RunningType = TradingType.Manual;
            portfolio.Quantity = 1;
            portfolio.UpdateDiff();
            portfolio.UpdateBalance();
            portfolio.Strategy = new Strategy() { StrategyID = Guid.NewGuid().ToString(), Name = "Auto1" };

            this.Add(portfolio);

            eqA = new EquityItem()
            {
                Code = "cu1205",
                Name = "沪铜1205",
                Side = Side.Short,
                PrevClose = 61580,
                Cost = 61860,
                Last = 61560
            };

            eqB = new EquityItem()
            {
                Code = "cu1206",
                Name = "沪铜1206",
                Side = Side.Long,
                PrevClose = 61740,
                Cost = 62020,
                Last = 61750
            };

            portfolio = new RunningPortfolioItem();
            portfolio.Equities.Add(eqA);
            portfolio.Equities.Add(eqB);
            portfolio.RunningType = TradingType.Auto;
            portfolio.Quantity = 1;
            portfolio.UpdateDiff();
            portfolio.UpdateBalance();
            portfolio.Strategy = new Strategy() { StrategyID = Guid.NewGuid().ToString(), Name = "Auto2" };

            this.Add(portfolio);

       }
    }
}
