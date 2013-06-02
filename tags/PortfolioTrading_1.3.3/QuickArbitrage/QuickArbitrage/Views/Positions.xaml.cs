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

namespace QuickArbitrage.Main.Views
{
    /// <summary>
    /// Interaction logic for Positions.xaml
    /// </summary>
    public partial class Positions : UserControl
    {
        public Positions()
        {
            InitializeComponent();
        }
    }

    class PositionsViewModel : ObservableCollection<PositionItem>
    {
        public PositionsViewModel()
        {
            this.Add(new PositionItem()
            {
                 Code = "cu1203",
                 Name = "沪铜1203",
                 Side = Side.Short,
                 Quantity = 2,
                 AvgCost = 61240,
                 Gain = -30
            });

            this.Add(new PositionItem()
            {
                Code = "cu1204",
                Name = "沪铜1204",
                Side = Side.Long,
                Quantity = 2,
                AvgCost = 61570,
                Gain = +50
            });
        }
        
    }
}
